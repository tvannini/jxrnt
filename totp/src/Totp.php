<?php
declare(strict_types=1);

/**
 * Implementazione TOTP (Time-based One-Time Password) — RFC 6238.
 *
 * TOTP è il meccanismo alla base delle app come Google Authenticator o Authy.
 * Il funzionamento in sintesi:
 *
 *   1. Server e utente condividono una chiave segreta casuale (generata una volta sola
 *      durante il setup e codificata in Base32 per essere leggibile/inseribile a mano).
 *
 *   2. Ogni 30 secondi, sia il server che l'app sul telefono calcolano lo stesso codice
 *      a 6 cifre usando quella chiave segreta + il tempo corrente come input a HMAC-SHA1.
 *
 *   3. L'utente inserisce il codice mostrato dall'app. Il server lo ricalcola e confronta.
 *      Poiché entrambi usano la stessa chiave e lo stesso intervallo di 30 secondi, i
 *      risultati coincidono — senza che il codice sia mai stato trasmesso.
 *
 * Questa classe è stateless: tutti i metodi sono statici.
 * Nessuna dipendenza esterna — usa solo hash_hmac() nativa di PHP.
 *
 * NOTA (modulo JXTOTP): file invariato rispetto al prototipo standalone —
 * l'algoritmo TOTP non ha nulla di specifico per l'integrazione Janox/ERP.
 */
class Totp
{
    /**
     * Alfabeto Base32 secondo RFC 4648: 26 lettere maiuscole + cifre 2-7 (totale 32 simboli).
     * Nota: non include 0, 1, 8, 9 per evitare confusione visiva con O, I, B, g.
     */
    const ALPHABET = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ234567';

    /** Numero di cifre nel codice OTP. 6 è il valore di default RFC 6238. */
    const DIGITS   = 6;

    /** Durata di un intervallo temporale in secondi. 30 è il default RFC 6238. */
    const PERIOD   = 30;

    /**
     * Genera una nuova chiave segreta TOTP casuale.
     *
     * Produce $bytes byte crittograficamente sicuri (tramite random_bytes(), che usa
     * /dev/urandom su Linux o CryptGenRandom su Windows) e li codifica in Base32.
     * Con 20 byte (160 bit) il risultato è una stringa di 32 caratteri Base32.
     *
     * Questa chiave viene salvata nel database e condivisa con l'utente tramite QR code.
     * Non deve mai essere rigenerata dopo la prima conferma: cambiarla invalida tutti
     * i dispositivi già configurati.
     */
    public static function generateSecret(int $bytes = 20): string
    {
        return self::base32Encode(random_bytes($bytes));
    }

    /**
     * Costruisce l'URI otpauth:// standard che il QR code deve contenere.
     *
     * Il formato è standardizzato (Google Key URI Format) e riconosciuto da tutte
     * le principali app authenticator:
     *
     *   otpauth://totp/<issuer>:<username>?secret=<BASE32>&issuer=<issuer>
     *
     * I parametri algorithm=SHA1, digits=6, period=30 vengono intenzionalmente omessi
     * perché corrispondono ai default RFC 6238 e la loro assenza riduce la lunghezza
     * dell'URI. URI più corti → QR code di versione inferiore → più facili da scansionare.
     *
     * <issuer> appare sia nel path (per compatibilità con app vecchie) sia come
     * query parameter (standard corrente). rawurlencode() è necessario per gestire
     * spazi e caratteri speciali nel nome dell'issuer o dello username.
     */
    public static function getUri(string $secret, string $username, string $issuer): string
    {
        return sprintf(
            'otpauth://totp/%s:%s?secret=%s&issuer=%s',
            rawurlencode($issuer),
            rawurlencode($username),
            $secret,
            rawurlencode($issuer)
        );
    }

    /**
     * Verifica se il codice OTP inserito dall'utente è valido.
     *
     * Oltre al codice per l'intervallo temporale corrente, vengono accettati anche
     * i codici degli intervalli adiacenti (±$window step = ±30 secondi per default).
     * Questo serve a compensare piccole differenze di orologio tra il telefono
     * dell'utente e il server, che in produzione possono arrivare a qualche decina
     * di secondi.
     *
     * Sicurezza: usa hash_equals() per il confronto, che impiega sempre lo stesso
     * tempo indipendentemente da quanto i due valori siano simili. Questo previene
     * i "timing attack", dove un attaccante misura il tempo di risposta per indovinare
     * cifra per cifra il codice corretto.
     */
    public static function verify(string $secret, string $code, int $window = 1): bool
    {
        // Rifiuta subito qualsiasi input che non sia esattamente 6 cifre.
        // Questo evita di fare confronti HMAC su input malformati.
        if (!preg_match('/^\d{6}$/', $code)) {
            return false;
        }

        // Contatore temporale: quanti intervalli di 30 secondi sono passati dall'epoch Unix.
        // Es. al secondo 1718800000, $t = 57293333.
        $t = (int) floor(time() / self::PERIOD);

        // Controlla il codice per l'intervallo corrente e per i $window intervalli
        // precedenti e successivi (con window=1: -30s, 0s, +30s).
        for ($i = -$window; $i <= $window; $i++) {
            $expected = self::hotp(self::base32Decode($secret), $t + $i);
            if (hash_equals($expected, $code)) {
                return true;
            }
        }

        return false;
    }

    /**
     * Calcola il codice OTP attuale (usato per test e debug).
     * Restituisce la stessa stringa che mostra l'app authenticator in questo momento.
     */
    public static function getCurrentCode(string $secret): string
    {
        $t = (int) floor(time() / self::PERIOD);
        return self::hotp(self::base32Decode($secret), $t);
    }

    // ── Metodi interni ─────────────────────────────────────────────────────────

    /**
     * Algoritmo HOTP (HMAC-based OTP) — RFC 4226, §5.
     *
     * Dato un segreto binario e un contatore intero, produce sempre la stessa
     * stringa di 6 cifre. In TOTP il contatore è il numero dell'intervallo temporale.
     *
     * Il processo in 3 fasi:
     *
     *   FASE 1 — HMAC-SHA1
     *     Il contatore viene serializzato come intero a 64 bit big-endian (8 byte).
     *     pack('J', $counter) fa esattamente questo ('J' = unsigned 64-bit big-endian).
     *     HMAC-SHA1 produce un digest di 20 byte usando la chiave segreta.
     *
     *   FASE 2 — Dynamic Truncation
     *     L'ultimo byte del digest (byte 19) ha i 4 bit bassi che fungono da "offset"
     *     (valore 0-15). Partendo da quell'offset, si prendono 4 byte consecutivi del
     *     digest. Il bit più significativo del primo byte viene azzerato (& 0x7F) per
     *     evitare ambiguità tra interi con e senza segno. I 4 byte formano un intero
     *     a 32 bit: questo è il "dynamic binary code".
     *
     *   FASE 3 — Riduzione a 6 cifre
     *     Modulo 1.000.000 riduce il numero nell'intervallo 0–999999.
     *     str_pad aggiunge zeri iniziali per garantire sempre 6 cifre (es. "000042").
     */
    private static function hotp(string $keyBytes, int $counter): string
    {
        // Fase 1: serializza il contatore in 8 byte big-endian e calcola HMAC-SHA1.
        // true come quarto parametro di hash_hmac → restituisce binario grezzo (20 byte)
        // invece di una stringa esadecimale.
        $msg  = pack('J', $counter);
        $hmac = hash_hmac('sha1', $msg, $keyBytes, true);

        // Fase 2: dynamic truncation.
        // I 4 bit bassi del byte 19 selezionano l'offset (0-15).
        $offset = ord($hmac[19]) & 0x0F;

        // Assembla 4 byte in un intero, azzerando il bit di segno del primo.
        $code   = (
            ((ord($hmac[$offset])     & 0x7F) << 24) |
            ((ord($hmac[$offset + 1]) & 0xFF) << 16) |
            ((ord($hmac[$offset + 2]) & 0xFF) << 8)  |
            ((ord($hmac[$offset + 3]) & 0xFF))
        ) % 1000000;

        // Fase 3: 6 cifre con zero-padding iniziale se necessario.
        return str_pad((string) $code, self::DIGITS, '0', STR_PAD_LEFT);
    }

    /**
     * Codifica una stringa binaria in Base32 (RFC 4648).
     *
     * Base32 usa 5 bit per carattere (2^5 = 32 simboli possibili).
     * Il processo accumula i bit in un buffer e ne estrae gruppi di 5
     * ogni volta che ce ne sono abbastanza.
     *
     * Esempio: 3 byte (24 bit) → 4 gruppi da 5 bit + 4 bit avanzati → 5 caratteri Base32.
     *
     * In questo contesto, Base32 è preferito a Base64 per il secret TOTP perché
     * usa solo lettere maiuscole e cifre 2-7, riducendo gli errori di digitazione
     * manuale e la confusione visiva (niente 0/O, 1/I, ecc.).
     */
    public static function base32Encode(string $data): string
    {
        $out      = '';
        $len      = strlen($data);
        $buffer   = 0;   // buffer accumulatore di bit
        $bitsLeft = 0;   // quanti bit validi ci sono nel buffer

        for ($i = 0; $i < $len; $i++) {
            // Aggiungi 8 bit (1 byte) al fondo del buffer.
            $buffer   = ($buffer << 8) | ord($data[$i]);
            $bitsLeft += 8;

            // Finché ci sono almeno 5 bit nel buffer, estraili e converti in carattere.
            while ($bitsLeft >= 5) {
                $bitsLeft -= 5;
                // Sposta a destra per mettere i 5 bit desiderati in posizione bassa,
                // poi maschera con 0x1F (= 11111 in binario) per prendere solo quei 5.
                $out .= self::ALPHABET[($buffer >> $bitsLeft) & 0x1F];
            }
        }

        // Se rimangono bit parziali (< 5), completali con zeri a destra e aggiungi l'ultimo carattere.
        if ($bitsLeft > 0) {
            $out .= self::ALPHABET[($buffer << (5 - $bitsLeft)) & 0x1F];
        }

        // Nota: il padding '=' di RFC 4648 viene omesso intenzionalmente.
        // Le app authenticator accettano Base32 senza padding.
        return $out;
    }

    /**
     * Decodifica una stringa Base32 in binario.
     *
     * Operazione inversa di base32Encode(): converte ogni carattere nel suo valore
     * a 5 bit e li accumula per ricostruire i byte originali.
     *
     * È tollerante rispetto a:
     *   - Lettere minuscole (convertite a maiuscole)
     *   - Caratteri '=' di padding (rimossi)
     *   - Caratteri non validi nell'alfabeto (saltati silenziosamente)
     */
    public static function base32Decode(string $data): string
    {
        // Rimuovi padding ed eventuali spazi, converti in maiuscolo.
        $data = strtoupper(rtrim($data, '='));

        // Costruisce una mappa inversa: carattere → valore 0-31.
        // array_flip(['A','B',...]) produce ['A'=>0, 'B'=>1, ...].
        $map      = array_flip(str_split(self::ALPHABET));
        $out      = '';
        $buffer   = 0;
        $bitsLeft = 0;

        for ($i = 0, $len = strlen($data); $i < $len; $i++) {
            $ch = $data[$i];
            // Salta i caratteri non riconosciuti senza generare errori.
            if (!isset($map[$ch])) {
                continue;
            }

            // Aggiungi 5 bit al buffer.
            $buffer    = ($buffer << 5) | $map[$ch];
            $bitsLeft += 5;

            // Appena ci sono almeno 8 bit, estrai un byte.
            if ($bitsLeft >= 8) {
                $bitsLeft -= 8;
                $out .= chr(($buffer >> $bitsLeft) & 0xFF);
            }
        }

        return $out;
    }
}
