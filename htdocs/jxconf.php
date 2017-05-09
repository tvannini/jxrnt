<html>
    <head>
        <style type="text/css">
        <!--
        * { font-family: "Courier New", Mono; color: #993300; font-size: 14px; box-sizing: border-box; }
        body {
         margin: 0 0 0 10px;
         }
        .rem {
         color: #414141;
         font-style: italic;
         padding: 0 0 0 30px;
         white-space: pre;
         }
         -->
        </style>
    </head>
    <body>
    <code><?php

     $ls = file("../janox.ini", FILE_IGNORE_NEW_LINES);
     if (count($ls)) {
        foreach ($ls as $i => $line) {
            if (!$line) {
                echo "<br/>";
                }
            elseif (substr(ltrim($line), 0, 1) == ";") {
                echo '<p class="rem">'.htmlspecialchars(substr(ltrim($line), 1))."</p>\n";
                }
            else {
                echo '<p>'.htmlspecialchars($line)."</p>\n";
                }
            }
        }
    ?>
    </code>
    </body>
</html>