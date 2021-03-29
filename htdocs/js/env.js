/**
 * Janox JavaScript Environment
 * JavaScript 5
 *
 *
 * This file is part of Janox.
 *
 * Janox is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 *
 * Janox is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * This script contains Janox JavaScript Environment, the Janox WEB client side
 * core runtime.
 *  .: Environment configuration functions
 *  .: Dynamic HTML
 *  .: Ajax style functions
 *  .: POST/GET information management
 *
 * @name      jxjse
 * @package   jxrnt/htdocs/js/env.js
 * @version   2.6
 * @copyright Tommaso Vannini (tvannini@janox.it) 2007
 * @author    Tommaso Vannini (tvannini@janox.it)
 */


String.prototype.trim    = function() { return this.replace(/^\s+|\s+$/g, ""); };
String.prototype.ltrim   = function() { return this.replace(/^\s+/, ""); };
String.prototype.rtrim   = function() { return this.replace(/\s+$/, ""); };


/**
 * Implement string split by separator with max chunks limit
 *
 * @param  {String}  separator
 * @param  {Integer} limit
 * @return {Array}
 */
String.prototype.explode = function(separator, limit) {

                               var arr = this.split(separator);
                               if (limit) {
                                   arr.push(arr.splice(limit - 1).join(separator));
                                   }
                               return arr;

                               };


/**
 * Decode HTML entities in a text, if any, using an TEXTAREA tag
 *
 */
String.prototype.decode = function() {

    if (o2jse.decoderObj == null) {
        o2jse.decoderObj = document.createElement("textarea");
        }
    o2jse.decoderObj.innerHTML = this;
    var returnText             = o2jse.decoderObj.value;
    o2jse.decoderObj.innerHTML = "";
    return returnText;

    };


/**
 * Instance of o2jse - Top level ref object for Janox JavaScript environment.
 * Definition in o2jse.init().
 * Provides desktop functionalities and ref root for all jxjs objects.
 *
 */
var o2jse = {

    rntAlias        : "/janox/",   /* Alias of virtual host where is Janox runtime      */
    appAlias        : "/",         /* Alias of virtual host where is Janox application  */
    charSet         : "UTF-8",     /* Page character set                                */
    sessName        : "PHPSESSID", /* PHP session name                                  */
    instId          : '',          /* Application instance ID (used by keep-alive)      */
    user            : "default",   /* Currently logged user                             */
    superUser       : false,       /* If user is logged as "root" (or is DEVELOPER)     */
    dev             : false,       /* Developer tools collection or FALSE if no devmode */
    decimalsPoint   : ',',         /* Character to use as decimal point separator       */
    thousandsPoint  : ' ',         /* Character to use as thousands point separator     */
    keepAlive       : 0,           /* Number of seconds for keep-alive sending (0=OFF)  */
    keepAliveHdl    : null,        /* Keep-alive timeout handler */
    refreshTime     : 0,           /* Number of seconds for refresh count-down (0=OFF)  */
    maxMultiReq     : 5,           /* Max allowed number of unresponsed requests        */
    menuStyle       : 'T',         /* Menu style [T]op or [L]eft                        */
    doc             : false,       /* If JXDocumentor is anabled for application        */
    profiling       : false,       /* If profiling capabilities enabled and mode (M|C|B)*/
    cliMode         : true,        /* If Full-AJAX behavior is enabled                  */
    waitOnCtrl      : true,        /* If waiting icon has to be displayed on last ctrl  */
    netErr          : false,       /* If network errors reporting is enabled            */
    fastMsgTime     : 5,           /* Fast messages timeout to fade away                */
    contMenuApp     : false,       /* If context menu shows application menu            */
    contMenuNewSess : false,       /* If context menu shows "Open new session"          */
    contMenuHelp    : false,       /* If context menu shows Help request                */
    started         : false,       /* If o2jse.init() method has already been called    */
    submitting      : false,       /* If o2jse is already submitting post data          */
    submitCtrl      : false,       /* Object that started submitting (submit-on-change) */
    safeChanges     : [],          /* List of controls changed since last submit-on-chg */
    infoForm        : null,        /* Reference to o2form_info form                     */
    elBody          : null,        /* HTML BODY element for the document                */
    glassObj        : null,        /* Full screen "glass" while objects moving & sizing */
    waitObj         : null,        /* DIV element containing wait-image during request  */
    clockObj        : null,        /* DIV shown while waiting                           */
    decoderObj      : null,        /* TEXTAREA object used to decode HTML entities      */
    runnerObj       : null,        /* SCRIPT object used to eval JS code                */
    reqData         : {},          /* List of posted information in previous request    */
    exeTree         : [],          /* Executions chain: running programs and actions    */
    keyList         : [],          /* List of enabled control keys                      */
    reposWins       : [],          /* List of windows to be repositioned                */
    waitingScripts  : 0,           /* Scripts waiting to be evaluated                   */
    maximizedWin    : false,       /* Maximized active form name for browser win resize */
    winResizing     : false,       /* Resizing event on browser window                  */
    closeLogout     : true,        /* Executing logout on browser window close          */
    closeWait       : false        /* Waiting for keyboard to close page (see init)     */

    };


/**
 * Client browser configurations and informations
 *
 */
o2jse.cli = {

    agent  : navigator.userAgent,  /* Client browser agent signature                    */
    width  : 0,                    /* Browser window width                              */
    height : 0                     /* Browser window height                             */

    };


/**
 * Sets up environment.
 * This function should be placed in the onLoad event of document.body
 *
 */
o2jse.init = function() {

    // ______________________________________ Get character-set from META Content-Type ___
    var metas = document.getElementsByTagName("meta");
    for (var i = 0; i < metas.length; i++) {
        if (metas[i].httpEquiv == "Content-Type") {
            var c = metas[i].content;
            o2jse.charSet = c.substring(c.lastIndexOf("=") + 1);
            break;
            }
        }
    // ____________________________________________________ Set environment properties ___
    o2jse.appAlias   = location.protocol + "//" +
                       location.host +
                       location.pathname.substr(0, location.pathname.lastIndexOf("/")) +
                       "/";
    o2jse.elBody     = (document.body || document.getElementsByTagName("body")[0]);
    o2jse.infoForm   = document.forms.o2form;
    o2jse.cli.width  = (window.innerWidth != null ? window.innerWidth :
                       (document.documentElement && document.documentElement.clientWidth ?
                        document.documentElement.clientWidth : o2jse.elBody.clientWidth));
    o2jse.cli.height = (window.innerHeight != null ? window.innerHeight :
                       (document.documentElement &&
                        document.documentElement.clientHeight ?
                        document.documentElement.clientHeight :
                        o2jse.elBody.clientHeight));
    o2jse.infoForm['o2_action'].value   = '';
    o2jse.infoForm['o2lastform'].value  = '';
    o2jse.infoForm['o2lastctrl'].value  = '';
    o2jse.submitting                    = false;
    o2jse.started                       = true;
    // ________________________________________________________________ Init all stuff ___
    if (jxjs.respTimeOut) {
        clearTimeout(jxjs.respTimeOut);
        jxjs.respTimeOut = null;
        }
    // ___________________________________________________________ Init AJAX Requester ___
    o2jse.requester.init();
    // ___________________________________________________________ Development console ___
    if (o2jse.devName && !o2jse.lab.contWin) {
        o2jse.lab.init();
        }
    // ============================================= Register partners in context menu ===
    o2jse.cMenu.partnersB = [];
    o2jse.cMenu.partnersA = [];
    // _________________________________________ Register calendar to use context menu ___
    o2jse.cMenu.regPartner(o2jse.cale.initContMenu, true);
    // _____________________ Register grid columns personalization to use context menu ___
    if (o2jse.tab.gridPlus) {
        o2jse.cMenu.regPartner(o2jse.tab.initContMenu, true);
        }
    // _________________________________ Register treeview control to use context menu ___
    o2jse.cMenu.regPartner(o2jse.tv.initContMenu, true);
    // __________________________________ Register developer tools to use context menu ___
    if (o2jse.devName) {
        o2jse.cMenu.regPartner(o2jse.dev.addDevMenu);
        }
    // _____________________________________ Register help request to use context menu ___
    if (o2jse.contMenuHelp) {
        o2jse.cMenu.regPartner(o2jse.docInitContMenu);
        }
    // ________________________________ Manage right-click events to open context menu ___
    document.oncontextmenu = o2jse.cMenu.setOn;
    // ________________________________________________________________ Set keep-alive ___
    if (o2jse.keepAlive && !o2jse.keepAliveHdl) {
        o2jse.keepAliveExe = function() {
                                jxjs.beacon('keepalive', { 'instid' : o2jse.instId });
                                clearTimeout(o2jse.keepAliveHdl);
                                o2jse.keepAliveHdl = setTimeout(o2jse.keepAliveExe,
                                                                o2jse.keepAlive);
                                };
        o2jse.keepAliveExe();
        }
    // ___________________________________________________ Set refresh (update notify) ___
    if (o2jse.notify.timeOut) {
        clearTimeout(o2jse.notify.timeOut);
        o2jse.notify.timeOut = null;
        }
    if (o2jse.refreshTime) {
        o2jse.notify.timeOut = setTimeout(o2jse.notify.exeReq, o2jse.refreshTime);
        }
    // _____________________________________________________ Set progress bars refresh ___
    if (o2jse.progress.active && !o2jse.progress.timeOut) {
        o2jse.progress.timeOut = setTimeout(function() {
                                                delete o2jse.progress.timeOut;
                                                o2jse.requester.exe('progress',
                                                                    'JXSESSNAME=' +
                                                                    o2jse.sessName,
                                                                    o2jse.progress,
                                                                   o2jse.progress.getCode,
                                                                    true);
                                                },
                                            3000);
        }
    // ________________________________________________ Windows positioning and sizing ___
    o2jse.reposAllWins();
    window.onresize = function() {
                         clearTimeout(o2jse.winResizing);
                         o2jse.winResizing = setTimeout(o2jse.resizeBrowserWindow, 500);
                         };
    // _________________________________________________________ OnLoad portable logic ___
    o2jse.waitDocReady();
    // ______________________________________________ Application logout on page close ___
    if (o2jse.closeLogout) {
        window.onunload    = function() { jxjs.beacon('logout'); };
        window.onmouseover = function () { if (!o2jse.closeWait) {
                                              window.onunload = null; } };
        window.onmouseout  = function () { if (!o2jse.submitting) {
                                              window.onunload = function() {
                                                                   jxjs.beacon('logout');
                                                                   }; } };
        window.onclick     = function () { window.onunload = null;
                                           o2jse.closeWait = false; };
        document.onkeydown = function (e) {
            if (e.key == 'Control' || e.key == 'Alt') {
                if (!o2jse.submitting) {
                    window.onunload = function() { jxjs.beacon('logout'); };
                    o2jse.closeWait = true;
                    }
                }
            else {
                window.onunload = null;
                o2jse.closeWait = false;
                }
            };
        }
    // _____________________________________________________________ Set focus control ___
    if (o2jse.ctrl.focusCtrl) {
        o2jse.cmd.focus(o2jse.ctrl.focusCtrl, o2jse.ctrl.focusMode);
        }

    };


/**
 * Waits for all scripts to be ended, then executes the o2jse.docReady if any.
 *
 */
o2jse.waitDocReady = function() {

    if (o2jse.waitingScripts > 0) {
        setTimeout(o2jse.waitDocReady, 10);
        }
    else {
        if (o2jse.docReady) {
            o2jse.docReady();
            o2jse.docReady = null;
            }
        }

    };


/**
 * Manage resize events on browser window.
 * Uses timeout to run only once at the end of resizing.
 *
 */
o2jse.resizeBrowserWindow = function() {

    if (o2jse.started) {
        o2jse.cli.width  = (window.innerWidth != null ?
                            window.innerWidth :
                            (document.documentElement &&
                             document.documentElement.clientWidth ?
                             document.documentElement.clientWidth :
                             o2jse.elBody.clientWidth));
        o2jse.cli.height = (window.innerHeight != null ?
                            window.innerHeight :
                            (document.documentElement &&
                             document.documentElement.clientHeight ?
                             document.documentElement.clientHeight :
                             o2jse.elBody.clientHeight));
        o2jse.reposAllWins();
        if (o2jse.maximizedWin) {


            maxWin = document.getElementById(o2jse.maximizedWin[0] + '_' +
                                             o2jse.maximizedWin[1]);

            o2jse.ctrl.init(maxWin);
            formInfo = maxWin.o2;
            // ________________________________________________________ Refresh action ___
            var refrAct = formInfo.r;
            // Set <form>_jxcmd = '3' to remove 'maximized' flag on form (server-side) ___
            var cmdField   = o2jse.infoForm[o2jse.maximizedWin[0] +
                                            o2jse.maximizedWin[1] + "_jxcmd"];
            if (cmdField) {
                cmdField.value = '4';
                o2jse.infoForm['o2_modfields'].value+= cmdField.name + ";";
                // __________________________________________ Refresh action on resize ___
                jxjs.refresh(formInfo, refrAct);
                }
            }
        }

    };


/**
 * Execute a JS code in the global scope.
 *
 * NOTE: EVAL inside a TRY/CATCH statement, inside a SCRIPT tag.
 *       This allows variables global scope and catching exceptions (syntax errors too)
 *       inside EVAL.
 *
 * @param {String} codeToExe   Code to execute
 */
o2jse.exeCode = function(codeToExe) {

    o2jse.codeToExe      = codeToExe;
    o2jse.runnerObj      = document.createElement("script");
    o2jse.runnerObj.text = "try { eval(o2jse.codeToExe); o2jse.codeToExe = null; } " +
                           "catch(ex) { jxjs.cError(ex, o2jse.codeToExe); } ";
    document.getElementsByTagName("head")[0].appendChild(o2jse.runnerObj);

    };


/**
 * Decode HTML entities in a text, if any.
 *
 * @param {String} decodeText   Text to decode
 */
o2jse.textDecode = function(decodeText) {

    if (o2jse.decoderObj == null) {
        o2jse.decoderObj = document.createElement("textarea");
        }
    o2jse.decoderObj.innerHTML = decodeText;
    var returnText             = o2jse.decoderObj.value;
    o2jse.decoderObj.innerHTML = "";
    return returnText;

    };


/**
 * Sets execution ID for the o2JS environment.
 * Execution ID is the higher program ID running (o2_app->progressivo_istanze).
 *
 * @param {Integer} exeId   Execution ID to set for environment
 */
o2jse.setExeId = function(exeId) {

    document.forms.o2form['o2_prgexeid'].value = exeId;

    };


/**
 * Load a requested JavaScript script in the page
 *
 * @param {String} fileToLoad   JavaScript file to be loaded
 */
o2jse.loadScript = function(fileToLoad) {

    var hTag  = document.getElementsByTagName("head")[0];
    var sTags = hTag.getElementsByTagName("script");
    var addS  = true;
    for (i = 0; i < sTags.length; i++) {
        if (sTags[i].src == fileToLoad) {
            addS = false;
            break;
            }
        }
    if (addS) {
        o2jse.waitingScripts++;
        var se    = o2jse.createEl(hTag, "script");
        se.onload = function () { o2jse.waitingScripts--; };
        se.src    = fileToLoad;
        }

    };


/**
 * Load a requested CSS file in the page
 *
 * @param {String} fileToLoad   CSS file to be loaded
 * @param {String} media        Media type (screen, print, ...)
 */
o2jse.loadCSS = function(fileToLoad, media) {

    var hTag  = document.getElementsByTagName("head")[0];
    var lTags = hTag.getElementsByTagName("link");
    var addL  = true;
    for (i = 0; i < lTags.length; i++) {
        if (lTags[i].href == fileToLoad) {
            addL = false;
            break;
            }
        }
    if (addL) {
        var le  = o2jse.createEl(hTag, "link");
        le.rel  = "stylesheet";
        le.href = fileToLoad;
        if (media) {
            le.media = media;
            }
        }

    };


/**
 * Stop executions (click on btn, navigator, ...) to preserve waiting requests
 * (submit-on-change)
 *
 * @param {Object} eventObj   Event object originating the call, if any
 */
o2jse.stopExe = function(eventObj) {

    if (eventObj) {
        o2jse.event.std(eventObj).stop();
        }
    // _____________________________________________ Reset focus to submitting control ___
    setTimeout(function() { o2jse.cmd.focus(o2jse.submitCtrl); }, 200);
    // __________________________________________________ Close open combo-list if any ___
    o2jse.lu.listOff();
    o2jse.fastMsg.show("<center>ACTION BLOCKED</center>" +
                       "Press TAB or ENTER to validate changed data");
    return false;

    };


/**
 * Change the status-bar content
 *
 * @param {String} txt_code   HTML or text content to display
 */
o2jse.statusMsg = function(txt_code) {

    document.getElementById("o2statustext").innerHTML = txt_code;

    };


/**
 * Creates an HTML element and sets environment object binding
 *
 * @param  {Object} inObj     Parent object
 * @param  {String} elType    HTML element type
 * @param  {String} elClass   CSS class name
 * @param  {String} elHtml    Tag HTML content
 * @return {Object}
 */
o2jse.createEl = function(inObj, elType, elClass, elHtml) {

    var lEl = document.createElement(elType);
    if (elClass) {
        lEl.className = elClass;
        }
    if (elHtml) {
        lEl.innerHTML = elHtml;
        }
    if (inObj) {
        inObj.appendChild(lEl);
        }
    return lEl;

    };


/**
 * Creates an HTML INPUT element and sets environment object binding
 *
 * @param {Object}  inObj       Parent object
 * @param {String}  elType      INPUT type [TEXT|INPUT|HIDDEN] default HIDDEN
 * @param {String}  elClass     CSS class name
 * @param {String}  elValue     Value of INPUT field
 * @param {Sstring} fieldName   Name for INPUT field
 */
o2jse.createInput = function(inObj, elType, elClass, elValue, fieldName) {

    var lEl  = document.createElement("INPUT");
    lEl.type = (elType ? elType : "hidden");
    if (fieldName) {
        lEl.name = fieldName;
        }
    if (elValue) {
        lEl.value = elValue;
        }
    if (elClass) {
        lEl.className = elClass;
        }
    inObj.appendChild(lEl);
    return lEl;

    };


/**
 * Removes an HTML element from its parent
 *
 * @param {Object} objToRemove   Object to be removed
 */
o2jse.removeEl = function(objToRemove) {

    if (objToRemove) {
        if (typeof objToRemove == "string") {
            if (elementObj = document.getElementById(objToRemove)) {
                o2jse.removeEl(elementObj);
                }
            }
        else if (objToRemove.parentNode) {
            objToRemove.parentNode.removeChild(objToRemove);
            }
        }

    };


/**
 * Finds absolute position of an element, summing all object ancestors positions. Position
 * is returned as an object in the form:
 *
 *    {x : x_pos, y : y_pos}
 *
 * @param   {Object} targetObj   Object to find out position for
 * @returns {Object}
 */
o2jse.getPos = function(targetObj) {

    var pos   = { x : targetObj.offsetLeft, y : targetObj.offsetTop };
    var level = targetObj;
    // ________________________________________________________ Sum all parents offset ___
    while (level = level.offsetParent) {
        pos.x += level.offsetLeft;
        pos.y += level.offsetTop;
        }
    // ___________________________________________________ Subtract all parents scroll ___
    level = targetObj;
    while (level = level.parentNode) {
        if (level.tagName.toLowerCase() == "body") {
            break;
            }
        else {
            pos.x -= level.scrollLeft;
            pos.y -= level.scrollTop;
            }
        }
    return pos;

    };


/**
 * Check visibility of targetObj in its parent: if parent object is scrollable and if
 * targetObj is not visible then parent is scrolled to make it visible.
 *
 * @param {Object} targetObj   Object to make visible
 */
o2jse.showByScroll = function(targetObj) {

    if (targetObj.offsetTop < targetObj.parentNode.scrollTop + 20) {
        targetObj.parentNode.scrollTop = targetObj.offsetTop;
        }
    else if (targetObj.offsetTop >
             (targetObj.parentNode.scrollTop + targetObj.parentNode.offsetHeight - 20)) {
        targetObj.parentNode.scrollTop = targetObj.offsetTop +
                                         targetObj.parentNode.offsetHeight;
        }

    };


/**
 * Finds tab index value for requested field
 *
 * @param {Object} targetObj   INPUT field to find out tab index
 */
o2jse.getTabIndex = function(targetObj) {

    if (targetObj.tabIndex) {
        return targetObj.tabIndex;
        }
    else {
        for (var i = (o2jse.infoForm.elements.length - 1); i > -1; i--) {
            if (o2jse.infoForm.elements[i] == targetObj) {
                return i;
                }
            }
        return -1;
        }

    };


/**
 * Sets tab index value for requested field
 *
 * @param {Object}  targetObj   INPUT field to find out tab index
 * @param {Integer} newIndex    New tab index to set for field
 */
o2jse.setTabIndex = function(targetObj, newIndex) {

    if (newIndex < 0) {
        newIndex = 0;
        }
    if (newIndex > (o2jse.infoForm.elements.length - 1)) {
        newIndex = (o2jse.infoForm.elements.length - 1);
        }
    var upIndex = 0;
    for (var i = (o2jse.infoForm.elements.length - 2); i > -1; i--) {
        upIndex = (i >= newIndex ? 1 : 0);
        if (o2jse.infoForm.elements[i] != targetObj) {
            o2jse.infoForm.elements[i].tabIndex = i + upIndex;
            }
        }
    targetObj.tabIndex = newIndex;

    };


/**
 * Looks for first "real" node child, skipping possible #text nodes
 *
 * @param {Object} parentObj   Object to look in for children
 */
o2jse.firstRealChild = function(parentObj) {

    var fieldLocal = parentObj.childNodes[0];
    while (fieldLocal.nodeType != 1) {
        if (!(fieldLocal = fieldLocal.nextSibling)) {
            break;
            }
        }
    return fieldLocal;

    };


/**
* Get the closest parent with the given tag name.
*/
o2jse.getParentTag = function(obj, tag) {

    var obj_parent = obj.parentNode;
    if (!obj_parent) {
        return false;
        }
    else if (obj_parent.tagName.toLowerCase() == tag.toLowerCase()) {
        return obj_parent;
        }
    else {
        return o2jse.getParentTag(obj_parent, tag);
        }

    };


/**
 * Sets right windows position on resize.
 * This method is also used to set right position for first displayed window after login,
 * because we don't have real browser dimensions yet.
 *
 */
o2jse.reposAllWins = function() {

    for (var winIndex in o2jse.reposWins) {
        o2jse.win.repos(o2jse.reposWins[winIndex]);
        }

    };


/**
 * Environment configuration utility functions collection.
 * Configuration functions are supposed to be called before o2jse.init() method.
 *
 */
o2jse.conf = {};


/**
 * Sets Janox runtime web path
 *
 * @param {String} aliasStr   Alias to set
 */
o2jse.conf.rntAlias = function(aliasStr) {

    o2jse.rntAlias = aliasStr;

    };


/**
 * Sets current PHP session name
 *
 * @param {String} phpSessName   PHP session name
 */
o2jse.conf.sessName = function(phpSessName) {

    o2jse.sessName = phpSessName;

    };


/**
 * Sets logout execution on close or browsing away from application page
 *
 * @param {String} phpSessName   PHP session name
 */
o2jse.conf.closeLogout = function(closeOnLogout) {

    o2jse.closeLogout = (closeOnLogout ? true : false);

    };


/**
 * Sets keep-alive count-down to force reload
 *
 * @param {Integer} tSecs   Number of seconds of the count-down
 */
o2jse.conf.keepAlive = function(tSecs, instId) {

    o2jse.keepAlive = tSecs * 1000;
    o2jse.instId    = instId;

    };


/**
 * Sets refresh time to call refresh program (update notify)
 *
 * @param {Integer} tSecs   Number of seconds of the count-down
 */
o2jse.conf.setRefresh = function(tSecs) {

    o2jse.refreshTime = tSecs * 1000;

    };


/**
 * Sets the maximum number of unresponded requests user is allowed to start
 *
 * @param {Integer} reqs   Number of requests
 */
o2jse.conf.setMaxReq = function(reqs) {

    o2jse.maxMultiReq = reqs;

    };


/**
 * Displays a waiting icon on submitting control.
 * It's used both in Full-AJAX ON and OFF.
 *
 * @param {Boolean} wait_on_ctrl   If waiting icon has to be displayed
 */
o2jse.conf.setWaiting = function(wait_on_ctrl) {

    o2jse.waitOnCtrl = (wait_on_ctrl ? true : false);

    };


/**
 * Set timeout for fast messages to fade away
 * It's used both in Full-AJAX ON and OFF.
 *
 * @param {Integer} timeOut   Time before fading away
 */
o2jse.conf.setFastMsgTime = function(timeOut) {

    o2jse.fastMsgTime = timeOut;

    };


/**
 * Enables JXDocumentor for application
 *
 */
o2jse.conf.doc = function() {

    o2jse.doc = true;

    };


/**
 * Set main menu style
 *
 * @param {String} mStyle   [T]op or [L]eft
 */
o2jse.conf.menuStyle = function(mStyle) {

    o2jse.menuStyle = mStyle;

    };


/**
 * Set context menu options visibility
 *
 * @param {Object} confList   List object of configuration parameters
 */
o2jse.conf.contextMenu = function(confList) {

    o2jse.contMenuApp     = confList.app;
    o2jse.contMenuNewSess = confList.newsess;
    o2jse.contMenuHelp    = confList.help;

    };


/**
 * Sets currently logged user
 *
 * @param {String} userId   Logged user
 */
o2jse.conf.user = function(userId) {

    o2jse.user = userId;
    if (userId.toLowerCase() == "root") {
        o2jse.superUser = true;
        }

    };


/**
 * Sets the currently logged developer name and starts development mode for application
 * test
 *
 * @param {String} devName   Developer name
 */
o2jse.conf.devel = function(devName) {

    o2jse.devName   = devName;
    o2jse.superUser = true;

    };


/**
 * Sets o2 runtime string to be used as decimals separator
 *
 * @param string pointStr   Separator to be used
 */
o2jse.conf.decimalsPoint = function(pointStr) {

    o2jse.decimalsPoint = pointStr;

    };


/**
 * Sets o2 runtime string to be used as thousands separator
 *
 * @param string pointStr   Separator to be used
 */
o2jse.conf.thousandsPoint = function(pointStr) {

    o2jse.thousandsPoint = pointStr;

    };


/**
 * Enables runtime profiling capabilities for SuperUser
 *
 * @param string profLevel   Profiling level ([M]enu | [C]ontrol | [B]oth)
 */
o2jse.conf.setProfiling = function(profLevel) {

    o2jse.profiling = profLevel;

    };


/**
 * Adds a key to the list of controlled keys and links it to an action
 *
 * @param integer keyCode    ASCII code of key to control
 * @param boolean shiftBtn   If SHIFT button has to be pressed
 * @param boolean ctrlBtn    If CTRL button has to be pressed
 * @param boolean altBtn     If ALT button has to be pressed
 */
o2jse.conf.addKey = function(keyCode, shiftBtn, ctrlBtn, altBtn, actCode) {

    o2jse.keyList.push({code  : keyCode,
                        shift : shiftBtn,
                        ctrl  : ctrlBtn,
                        alt   : altBtn,
                        exe   : actCode});

    };


/**
 * Set db-grid control extra functions
 *
 * @param {Boolean} autoSave   Auto or manual settings saving mode
 * @param {Boolean} multiDel   Enables multi row deletion from context menu
 */
o2jse.conf.gridPlus = function(autoSave, multiDel) {

    o2jse.tab.gridPlus         = true;
    o2jse.tab.autoSaveSettings = autoSave;
    o2jse.tab.gridMultiDel     = multiDel;

    };


/**
 * Set db-grid action on double click
 *
 * @param {String} act   Action to be executed []Nothing, [D]etail, [S]elect, [B]oth
 */
o2jse.conf.gridDblClick = function(act) {

    o2jse.tab.dblClickAct = act;

    };


/**
 * Set db-grid paging by wheel
 *
 * @param {Boolean} wheel   If TRUE enables wheel use in grid
 */
o2jse.conf.gridWheel = function(wheel) {

    o2jse.tab.wheelPaging = (wheel ? true : false);

    };


/**
 * Set lookup control request delay time.
 * This is the time after which request will be fired while typing.
 * Minimum allowed time is 300 ms.
 *
 * @param {Boolean} reqTime   Timein milliseconds
 */
o2jse.conf.luReqTime = function(reqTime) {

    o2jse.lu.reqDelay = (reqTime >= 300 ? reqTime : 300);

    };


/**
 * Set on network errors reporting
 *
 */
o2jse.conf.netErr = function() {

    o2jse.netErr = true;

    };


/**
 * Object managing background requests to server by XMLHttpRequest
 *
 */
o2jse.requester = {

    reqList : []

    };


o2jse.requester.init = function() {

    this.mon.init();

    };


o2jse.requester.startReq = function(reqId) {

    var newReq = {

        id        : reqId,
        engine    : new XMLHttpRequest(),
        sourceObj : null

        };

    newReq.engine.open("POST", o2jse.rntAlias + "jxr.php", true);
    newReq.engine.setRequestHeader("Content-Type",
                                   "application/x-www-form-urlencoded; charset=" +
                                   o2jse.charSet);
    this.reqList.push(newReq);
    this.mon.refresh();
    if (this.reqList.length >= o2jse.maxMultiReq) {
        o2jse.cmd.showClock();
        }
    return newReq;

    };


o2jse.requester.endReq = function(reqId) {

    for (var i = this.reqList.length - 1; i >= 0; i--) {
        if (this.reqList[i].id == reqId) {
            this.reqList.splice(i, 1);
            break;
            }
        }
    if (this.reqList.length < 1) {
        o2jse.cmd.hideClock();
        }
    this.mon.refresh();

    };


/**
 * Create request for current page by XMLHttpRequest
 *
 * @param string   action      Function to execute on server (lookup, dispatcher, ...):
 *                             pass it as blank for pure page POST.
 * @param string   addToBody   String to be added to request body
 * @param object   fromObj     Object requesting action: it is stored on the request
 *                             object so to refer it later in callback function
 * @param function callBack    Function managing request end
 * @param boolean  noFields    Don't add page fields and pass only addToBody code
 */
o2jse.requester.exe = function(action, addToBody, fromObj, callBack, noFields = false) {

    var reqId    = "_" + new Date().getTime();
    var postBody = "";
    if (!noFields) {
        for (var fieldId = o2jse.infoForm.elements.length; fieldId > 0; fieldId--) {
            var singleField = o2jse.infoForm.elements[fieldId - 1];
            // __________________ Questo permette di inviare sempre lo stesso pagemark ___
            if (singleField.name &&
                !singleField.disabled &&
                singleField.name != "o2pagemark") {
                postBody += (singleField.name + "=" +
                             encodeURIComponent(singleField.value) + "&");
                }
            }
        }
    // ________________________________________________________________ Create request ___
    var reqObj = this.startReq(reqId);

    /**
     * Handler for request return
     *
     */
    reqObj.engine.onreadystatechange = function() {

        if (reqObj.engine.readyState == 4) {
            if (reqObj.engine.status == 0) {
                st = reqObj.engine.status;
                o2jse.requester.endReq(reqId);
                // ________________________ Exclude requests from refresh (dispatcher) ___
                if (fromObj !== o2jse.notify) {
                    // _________________________________________ Report network errors ___
                    if (o2jse.netErr) {
                        alert("Network error: please try again.");
                        }
                    }
                else {
                    // _________________________________________ Reset refresh timeout ___
                    if (o2jse.notify.timeOut) {
                        clearTimeout(o2jse.notify.timeOut);
                        o2jse.notify.timeOut = null;
                        }
                    o2jse.notify.timeOut   = setTimeout(o2jse.notify.exeReq,
                                                        o2jse.refreshTime);
                    o2jse.notify.inRequest = false;
                    }
                }
            else {
                var resText = reqObj.engine.responseText;
                o2jse.requester.endReq(reqId);
                // __ Function is called passing original object and request body text ___
                if (callBack) {
                    callBack(fromObj, resText, reqId);
                    }
                }
            }

        };

    reqObj.engine.send(postBody + "jxact=" + (action || "pagepost") +
                       (addToBody ? "&" + addToBody : ""));
    return reqId;

    };


/**
 * Requester monitor: visual indicator displaying pending and actually solved requests
 *
 */
o2jse.requester.mon = {

    outBox    : null,
    leds_n    : 5,
    leds      : []

    };


/**
 * Initialize monitor for ajax request/response
 *
 */
o2jse.requester.mon.init = function() {

    if (!(document.getElementById('jxReqMon'))) {
        this.outBox    = o2jse.createEl(o2jse.elBody, 'DIV', 'reqMon');
        this.outBox.id = 'jxReqMon';
        this.leds_n    = o2jse.maxMultiReq;
        this.leds[0]   = o2jse.createEl(this.outBox, 'DIV', 'reqMonFree');
        for (var i = 1; i < this.leds_n; i++) {
            this.leds[i] = o2jse.createEl(this.outBox, 'DIV', 'reqMonOff');
            }
        }

    };


/**
 * Refresh monitor to set leds ON/OFF
 *
 */
o2jse.requester.mon.refresh = function() {

    var ledsOnCount = o2jse.requester.reqList.length;
    if (ledsOnCount < 1) {
        this.leds[0].className = 'reqMonFree';
        for (var i = 1; i < this.leds_n; i++) {
            this.leds[i].className = 'reqMonOff';
            }
        }
    else {
        if (ledsOnCount < (this.leds_n + 1)) {
            for (var i = 0; i < this.leds_n; i++) {
                this.leds[i].className = (i < ledsOnCount  ? 'reqMonOn' : 'reqMonOff');
                }
            }
        }

    };


/**
 * Events utility functions collection
 *
 */
o2jse.event = {};


/**
 * Returns standardized event object from different browsers
 *
 * @param object eventObj   Event object passed by event to handler function
 */
o2jse.event.std = function(eventObj) {

    if (window.event) {
        // _____________________________________________________________ Google Chrome ___
        o2jse.ctrl.init(window.event.srcElement);
        return {
            target   : window.event.srcElement,
            x        : window.event.clientX,
            y        : window.event.clientY,
            offsetX  : window.event.offsetX,
            offsetY  : window.event.offsetY,
            button   : window.event.button,
            keyCode  : window.event.keyCode,
            charCode : window.event.charCode,
            ctrlKey  : window.event.ctrlKey,
            shiftKey : window.event.shiftKey,
            altKey   : window.event.altKey,
            delta    : -(window.event.wheelDelta / 120),
            stop     : function() {
                          window.event.returnValue  = false;
                          window.event.cancelBubble = true;
                          }
            };
        }
    // _______________________________________________________________________ FireFox ___
    else if (eventObj) {
        o2jse.ctrl.init(eventObj.target);
        return {
            target   : eventObj.target,
            x        : eventObj.clientX,
            y        : eventObj.clientY,
            offsetX  : eventObj.layerX - eventObj.target.offsetLeft,
            offsetY  : eventObj.layerY - eventObj.target.offsetTop,
            button   : eventObj.button,
            keyCode  : eventObj.keyCode,
            charCode : eventObj.charCode,
            ctrlKey  : eventObj.ctrlKey,
            shiftKey : eventObj.shiftKey,
            altKey   : eventObj.altKey,
            delta    : eventObj.detail / 3,
            stop     : function() {
                          eventObj.preventDefault();
                          eventObj.stopPropagation();
                          }
            };
        }
    else {
        return {
            target   : null,
            x        : 0,
            y        : 0,
            offsetX  : 0,
            offsetY  : 0,
            button   : 0,
            keyCode  : 0,
            charCode : 0,
            ctrlKey  : false,
            shiftKey : false,
            altKey   : false,
            stop     : function() {}
            };
        }

    };


/**
 * Controls events handlers collection
 *
 */
o2jse.ctrl = {

    focusCtrl     : null,  /* Control to get focus when o2jse starts                    */
    focusMode     : null,  /* Selection mode in focus when o2jse starts                 */
    caleObj       : null,  /* Active o2 calendar object                                 */
    editAreaList  : [],    /* List of active edit areas on page                         */
    upLoad        : false, /* If page contains files to upload (Full-AJAX force submit) */
    lastKeyCode   : 0,     /* Key code of last pressed key                              */
    focusFallBack : false  /* Control name to use as focus fall back (when focus lost)  */

    };


/**
 * Creates element JavaScript properties from "o2" attribute
 *
 * @param object obj2Init   Object to be initialized
 */
o2jse.ctrl.init = function(obj2Init) {

    if (typeof obj2Init.o2 != "object") {
        var hinO2 = hineritAttr(obj2Init);
        if (typeof hinO2 == "object") {
            obj2Init.o2 = hinO2;
            }
        else {
            if (typeof hinO2 == "string") {
                eval("obj2Init.o2={" + hinO2 + "};");
                }
            else {
                obj2Init.o2 = {};
                }
            }
        }

    function hineritAttr(hinObj) {

        if (typeof hinObj.o2 == "object") {
            return hinObj.o2;
            }
        else {
            if (hinObj.hasAttribute && hinObj.hasAttribute("o2")) {
                return hinObj.getAttribute("o2");
                }
            else {
                if (hinObj.parentNode &&
                    typeof hinObj.parentNode == "object" &&
                    hinObj.parentNode.tagName            &&
                    hinObj.parentNode.tagName.toLowerCase() != "body") {
                    return hineritAttr(hinObj.parentNode);
                    }
                else {
                    return "";
                    }
                }
            }

        }

    return true;

    };


/**
 * Executes actions matching keyboard shortcuts when pressed on controls of a form
 *
 * @param  string navBar
 * @param  object eventObj
 * @return boolean
 */
o2jse.ctrl.k = function(eventObj) {

    var stdEvent           = o2jse.event.std(eventObj);
    var keyCode            = stdEvent.keyCode;
    var singleKey          = null;
    o2jse.ctrl.lastKeyCode = keyCode;
    o2jse.ctrl.init(stdEvent.target);
    // ______________________________________ Special behaviours for specific controls ___
    switch (stdEvent.target.o2.cT) {
        // ____________________________________________________________________ Button ___
        case "button":
            // _______________________________ Activate buttons (space bar and return) ___
            if (keyCode == 32 || keyCode == 13) {
                stdEvent.target.click();
                return false;
                }
            // ___________________________________ Left arrow - Select previous button ___
            else if (keyCode == 37) {
                var btns    = o2jse.ctrl.getByJxType("button");
                var currBtn = stdEvent.target.o2.c;
                var prevBtn = currBtn;
                for (var i = 0; i < btns.length; i++) {
                    if (btns[i] == currBtn) {
                        if (i == 0) {
                            prevBtn = btns[btns.length - 1];
                            }
                        break;
                        }
                    else {
                        prevBtn = btns[i];
                        }
                    }
                document.getElementById(prevBtn + stdEvent.target.o2.e).focus();
                stdEvent.stop();
                }
            // ______________________________________ Right arrow - Select next button ___
            else if (keyCode == 39) {
                var btns    = o2jse.ctrl.getByJxType("button");
                var currBtn = stdEvent.target.o2.c;
                var nextBtn = currBtn;
                for (var i = btns.length - 1; i >= 0; i--) {
                    if (btns[i] == currBtn) {
                        if (btns[i] == nextBtn) {
                            nextBtn = btns[0];
                            }
                        break;
                        }
                    else {
                        nextBtn = btns[i];
                        }
                    }
                document.getElementById(nextBtn + stdEvent.target.o2.e).focus();
                stdEvent.stop();
                }
            break;
        case "edit":
            stdEvent.target.inEdit = true;
            break;
        case "text":
            stdEvent.target.inEdit = true;
            // _____________________ Preserve RETURN and cursor movements in text area ___
            if (keyCode == 13 || (keyCode >= 37 && keyCode <= 40)) {
                return false;
                }
            break;
        }
    // _________________________ Standard behaviours (all controls) - Mapped by options___
    for (var o2keyIndex = o2jse.keyList.length - 1; o2keyIndex >= 0 ; o2keyIndex--) {
        singleKey = o2jse.keyList[o2keyIndex];
        if (singleKey.code > 0                   &&
            singleKey.code  == keyCode           &&
            singleKey.shift == stdEvent.shiftKey &&
            singleKey.ctrl  == stdEvent.ctrlKey  &&
            singleKey.alt   == stdEvent.altKey) {
            var navName = "";
            if (stdEvent.target.o2.nav) {
                navName = stdEvent.target.o2.nav + String(stdEvent.target.o2.e);
                }
            switch (singleKey.exe) {
                case 1: // _______________________ Go to first record navigator button ___
                    if (navName) {
                        stdEvent.stop();
                        var navLocal = o2jse.infoForm[navName];
                        o2jse.ctrl.init(navLocal);
                        if (navLocal.o2.navNav) {
                            stdEvent.target.blur();
                            o2jse.nav.btnExe(document.getElementById(navName + "_fbtn"));
                            }
                        }
                    return false;
                    break;
                case 2: // ______________________ Go to previous page navigator button ___
                    if (navName) {
                        stdEvent.stop();
                        var navLocal = o2jse.infoForm[navName];
                        o2jse.ctrl.init(navLocal);
                        if (navLocal.o2.navNav) {
                            stdEvent.target.blur();
                            o2jse.nav.btnExe(document.getElementById(navName + "_ppbtn"));
                            }
                        }
                    return false;
                    break;
                case 3: // ____________________ Go to previous record navigator button ___
                    if (o2jse.cliMode &&
                       (stdEvent.target.o2.cT == "tab" ||
                        stdEvent.target.o2.pT == "tab")) {
                        o2jse.tab.changeRow(stdEvent.target, "P", navName);
                        stdEvent.stop();
                        return false;
                        }
                    if (navName) {
                        stdEvent.stop();
                        var navLocal = o2jse.infoForm[navName];
                        o2jse.ctrl.init(navLocal);
                        if (navLocal.o2.navNav) {
                            stdEvent.target.blur();
                            // _______________________________________ Selection table ___
                            if (stdEvent.target.o2.pointer) {
                                o2jse.tab.selectRow(true, stdEvent.target);
                                }
                            else {
                                o2jse.nav.btnExe(document.getElementById(navName +
                                                                         "_pbtn"));
                                }
                            }
                        }
                    return false;
                    break;
                case 4: // ________________________ Go to next record navigator button ___
                    if (o2jse.cliMode &&
                       (stdEvent.target.o2.cT == "tab" ||
                        stdEvent.target.o2.pT == "tab")) {
                        o2jse.tab.changeRow(stdEvent.target, "N", navName);
                        stdEvent.stop();
                        return false;
                        }
                    if (navName) {
                        stdEvent.stop();
                        var navLocal = o2jse.infoForm[navName];
                        o2jse.ctrl.init(navLocal);
                        if (navLocal.o2.navNav) {
                            stdEvent.target.blur();
                            // _______________________________________ Selection table ___
                            if (stdEvent.target.o2.pointer) {
                                o2jse.tab.selectRow(false, stdEvent.target);
                                }
                            else {
                                o2jse.nav.btnExe(document.getElementById(navName +
                                                                          "_nbtn"));
                                }
                            }
                        }
                    return false;
                    break;
                case 5: // __________________________ Go to next page navigator button ___
                    if (navName) {
                        stdEvent.stop();
                        var navLocal = o2jse.infoForm[navName];
                        o2jse.ctrl.init(navLocal);
                        if (navLocal.o2.navNav) {
                            stdEvent.target.blur();
                            o2jse.nav.btnExe(document.getElementById(navName + "_npbtn"));
                            }
                        }
                    return false;
                    break;
                case 6: // ________________________ Go to last record navigator button ___
                    if (navName) {
                        stdEvent.stop();
                        var navLocal = o2jse.infoForm[navName];
                        o2jse.ctrl.init(navLocal);
                        if (navLocal.o2.navNav) {
                            stdEvent.target.blur();
                            o2jse.nav.btnExe(document.getElementById(navName + "_lbtn"));
                            }
                        }
                    return false;
                    break;
                case 11: // ______________________________ New record navigator button ___
                    if (navName) {
                        stdEvent.stop();
                        var navLocal = o2jse.infoForm[navName];
                        o2jse.ctrl.init(navLocal);
                        if (navLocal.o2.navNew) {
                            stdEvent.target.blur();
                            o2jse.nav.btnExe(document.getElementById(navName +"_newbtn"));
                            }
                        }
                    return false;
                    break;
                case 12: // ___________________________ Delete record navigator button ___
                    if (navName) {
                        stdEvent.stop();
                        var navLocal = o2jse.infoForm[navName];
                        o2jse.ctrl.init(navLocal);
                        if (navLocal.o2.navDel) {
                            stdEvent.target.blur();
                            o2jse.nav.btnExe(document.getElementById(navName +"_delbtn"));
                            }
                        }
                    return false;
                    break;
                case 13: // _____________________________ Save record navigator button ___
                    if (navName) {
                        stdEvent.stop();
                        var navLocal = o2jse.infoForm[navName];
                        o2jse.ctrl.init(navLocal);
                        if (navLocal.o2.navSave) {
                            stdEvent.target.blur();
                            o2jse.nav.btnExe(document.getElementById(navName +
                                                                     "_savebtn"));
                            }
                        }
                    if (stdEvent.target.focus) {
                        stdEvent.target.focus();
                        if (stdEvent.target.select) {
                            stdEvent.target.select();
                            }
                        }
                    return false;
                    break;
                case 14: // _____________________________ Undo record navigator button ___
                    if (navName) {
                        stdEvent.stop();
                        var navLocal = o2jse.infoForm[navName];
                        o2jse.ctrl.init(navLocal);
                        if (navLocal.o2.navUndo) {
                            stdEvent.target.blur();
                            o2jse.nav.btnExe(document.getElementById(navName +
                                                                     "_undobtn"));
                            }
                        }
                    return false;
                    break;
                case 15: // ___________________________ Select record navigator button ___
                    if (navName) {
                        var navLocal = o2jse.infoForm[navName];
                        o2jse.ctrl.init(navLocal);
                        if (navLocal.o2.navSele) {
                            stdEvent.stop();
                            stdEvent.target.blur();
                            o2jse.submitting = false;
                            o2jse.nav.btnExe(document.getElementById(navName +
                                                                     "_selebtn"));
                            }
                        }
                    return false;
                    break;
                case 16: // _______________________ Details of record navigator button ___
                    if (navName) {
                        stdEvent.stop();
                        var navLocal = o2jse.infoForm[navName];
                        o2jse.ctrl.init(navLocal);
                        if (navLocal.o2.navDet) {
                            stdEvent.target.blur();
                            o2jse.nav.btnExe(document.getElementById(navName +"_detbtn"));
                            }
                        }
                    return false;
                    break;
                case 17: // _____________________________________________ Form refresh ___
                    stdEvent.stop();
                    stdEvent.target.blur();
                    o2jse.cmd.submit(stdEvent.target.o2.e);
                    return false;
                    break;
                case 18: // _____________________________________ o2 documentor (help) ___
                    stdEvent.stop();
                    o2jse.cmd.doc(stdEvent.target);
                    return false;
                    break;
                case 19: // _______________________________________________ Form close ___
                    stdEvent.stop();
                    stdEvent.target.blur();
                    o2jse.win.exit(stdEvent.target);
                    return false;
                    break;
                case 20: // _____________________________________________ Control zoom ___
                    return o2jse.ctrl.zoom(eventObj);
                    break;
                default: // _____________________________________ User defined actions ___
                    if (parseInt(singleKey.exe) != singleKey.exe) {
                        stdEvent.target.blur();
                        if (typeof stdEvent.target == "object" &&
                            stdEvent.target.value != stdEvent.target.defaultValue) {
                            o2jse.cmd.ctrlUpd(stdEvent.target);
                            }
                        var relatedBtn = document.getElementById(singleKey.exe +
                                                                 stdEvent.target.o2.e);
                        o2jse.ctrl.btnExe(relatedBtn);
                        }
                    return false;
                }
            break;
            }
        }
    return false;

    };


/**
 * Handler for OnFocus events on edit ad text fields
 *
 * @param object targetObj   Object on which event is fired
 */
o2jse.ctrl.f = function(targetObj) {

    o2jse.ctrl.init(targetObj);
    var objInfo = targetObj.o2;
    o2jse.infoForm['o2lastform'].value = objInfo.f;
    o2jse.infoForm['o2lastctrl'].value = objInfo.c;
    if (objInfo.cT != "text" && targetObj.select) {
        targetObj.select();
        // ______________________________________ Prevent onMouseUp to unset selection ___
        targetObj.onmouseup = function(e) {
                                 e = o2jse.event.std(e);
                                 e.stop();
                                 this.onmouseup = null;
                                 };
        }

    };


/**
 * Handler for OnBlur events on edit and text fields
 *
 * @param object targetObj   Object on which event is fired
 */
o2jse.ctrl.b = function(targetObj) {

    targetObj.inEdit = false;

    };


/**
 * Handler for OnChange events controls based on field
 *
 * @param object targetObj   Object on which event is fired
 */
o2jse.ctrl.c = function(targetObj) {

    o2jse.ctrl.init(targetObj);
    switch (targetObj.o2.dT) {
        // ______________________________________________________________________ ALFA ___
        case "A":
            if (targetObj.o2.cT == 'edit') {
                targetObj.value = targetObj.value.replace(/\s/g, " ");
                }
            targetObj.value = o2jse.data.fa(targetObj.value, targetObj.o2.m);
            break;
        // __________________________________________________________________  NUMERIC ___
        case "N":
            targetObj.value = o2jse.data.fn(targetObj.value, targetObj.o2.m);
            break;
        // ______________________________________________________________________ DATE ___
        case "D":
            targetObj.value = o2jse.data.fd(targetObj.value, targetObj.o2.m);
            break;
        // ______________________________________________________________________ TIME ___
        case "O":
            targetObj.value = o2jse.data.ft(targetObj.value, targetObj.o2.m);
            break;
        // ___________________________________________________________________ LOGICAL ___
        case "L":
            // ___________________ NOTE: Check boxes not here! See o2jse.cb.c() method ___
            targetObj.value = (targetObj.value ? "1" : "");
            break;
        // _____________________________________________________________________ OTHER ___
        default:
            s               = new String(targetObj.value);
            targetObj.value = s.toString();
            break;
        }
    targetObj.inEdit = false;
    if (targetObj.o2.fret) {
        o2jse.ctrl.make_waiting(targetObj);
        if (o2jse.cliMode) {
            var nF;
            // ___________________________________________________ Preserve focus flow ___
            nF = o2jse.createInput(o2jse.infoForm, "hidden", "", "1", "jxnofocus");
            o2jse.submitCtrl = targetObj.o2.c + targetObj.o2.e;
            jxjs.request(targetObj, targetObj.value);
            // _____________________________________________ Remove no-focus directive ___
            if (nF) {
                o2jse.removeEl(nF);
                }
            }
        else {
            o2jse.cmd.ctrlUpd(targetObj);
            }
        }
    else {
        o2jse.cmd.ctrlUpd(targetObj);
        }

    };


/**
 * Limit text length on key-press in textareas
 *
 * @param {Object} eventObj   Event fired on textarea to limit
 */
o2jse.ctrl.l =  function(eventObj) {

    var stdEvent = o2jse.event.std(eventObj);
    var ctrlObj  = stdEvent.target;
    o2jse.ctrl.init(ctrlObj);
    var maxSize = parseInt(ctrlObj.o2.s);
    // _______________ Text is shorter then maxSize or non-printable character pressed ___
    if ((ctrlObj.value.length < maxSize) || (stdEvent.charCode == 0)) {
        return true;
        }
    // _______________________________________ Trying to make text longer then maxSize ___
    else {
        return false;
        }

    };


/**
 * Display a waiting image inside a given control
 *
 * @param object  waitCtrl   Control object to fill with waiting image
 */
o2jse.ctrl.make_waiting = function(waitCtrl) {

    if (o2jse.waitOnCtrl) {
        if (o2jse.waitObj) {
            o2jse.removeEl(o2jse.waitObj);
            delete o2jse.waitObj;
            }
        if (jxjs.waitingCtrl) {
            jxjs.waitingCtrl.style.display = "block";
            }
        switch (waitCtrl.tagName.toUpperCase()) {
            case 'TD': // ______________________ Controls as a TD in TABLE (win-close) ___
                // ______________________________________________ Save waiting control ___
                jxjs.waitingCtrl = waitCtrl;
                // _______________________________________ Save current control values ___
                var pNode   = waitCtrl.parentNode;
                var cWidth  = waitCtrl.offsetWidth;
                var cHeight = waitCtrl.offsetHeight;
                var cClass  = waitCtrl.className;
                // ______________________________________________________ Hide control ___
                waitCtrl.style.display = 'none';
                // __________________________ Simulate original control as a container ___
                o2jse.waitObj              = pNode.insertCell(waitCtrl.cellIndex);
                o2jse.waitObj.className    = cClass;
                o2jse.waitObj.style.width  = cWidth + 'px';
                o2jse.waitObj.style.height = cHeight + 'px';
                // ___________________________ Create wait image inside pseudo-control ___
                var wObj            = o2jse.createEl(o2jse.waitObj,
                                                     'DIV',
                                                     'jx_inctrl_wait',
                                                     '&nbsp;');
                wObj.style.position = 'relative';
                wObj.style.left     = parseInt((cWidth - wObj.offsetWidth) / 2) + 'px';
                wObj.style.top      = '0';
                break;
            case 'TH': // ______________________ Controls as a TD in TABLE (win-close) ___
                // ______________________________________________ Save waiting control ___
                jxjs.waitingCtrl = waitCtrl;
                // _______________________________________ Save current control values ___
                var pNode   = waitCtrl.parentNode;
                var cWidth  = waitCtrl.offsetWidth;
                var cHeight = waitCtrl.offsetHeight;
                var cClass  = waitCtrl.className;
                // ______________________________________________________ Hide control ___
                waitCtrl.style.display = 'none';
                // __________________________ Simulate original control as a container ___
                o2jse.waitObj          = o2jse.createEl(false, 'TH', cClass);
                pNode.insertBefore(o2jse.waitObj, waitCtrl);
                o2jse.waitObj.style.width  = cWidth + 'px';
                o2jse.waitObj.style.height = cHeight + 'px';
                // ___________________________ Create wait image inside pseudo-control ___
                var wObj            = o2jse.createEl(o2jse.waitObj,
                                                     'DIV',
                                                     'jx_inctrl_wait',
                                                     '&nbsp;');
                wObj.style.position = 'relative';
                wObj.style.left     = parseInt((cWidth - wObj.offsetWidth) / 2) + 'px';
                wObj.style.top      = '0';
                break;
            case 'INPUT': // _____________ Controls as an INPUT (EDIT, TEXT-AREA, ...) ___
                o2jse.ctrl.b(waitCtrl);
                // ______________________________________________ Save waiting control ___
                jxjs.waitingCtrl = waitCtrl;
                // _______________________________________ Save current control values ___
                var pNode   = waitCtrl.parentNode;
                var cWidth  = waitCtrl.offsetWidth;
                var cHeight = waitCtrl.offsetHeight;
                var cClass  = waitCtrl.className;
                // ______________________________________________________ Hide control ___
                waitCtrl.style.display = "none";
                // __________________________ Simulate original control as a container ___
                o2jse.waitObj          = o2jse.createEl(false, 'DIV', cClass);
                pNode.insertBefore(o2jse.waitObj, waitCtrl.nextSibling);
                o2jse.waitObj.style.width  = cWidth + 'px';
                o2jse.waitObj.style.height = cHeight + 'px';
                // ___________________________ Create wait image inside pseudo-control ___
                var wObj            = o2jse.createEl(o2jse.waitObj,
                                                     'DIV',
                                                     'jx_inctrl_wait',
                                                     '&nbsp;');
                wObj.style.position = 'absolute';
                wObj.style.left     = parseInt((cWidth - wObj.offsetWidth) / 2) + 'px';
                wObj.style.top      = parseInt((cHeight - wObj.offsetHeight) / 2) + 'px';
                break;
            default: // ________ Controls as a DIV (navigator & buttons) and other tags___
                // ______________________________________________ Save waiting control ___
                jxjs.waitingCtrl = waitCtrl;
                // _______________________________________ Save current control values ___
                var pNode   = waitCtrl.parentNode;
                var cWidth  = waitCtrl.offsetWidth;
                var cHeight = waitCtrl.offsetHeight;
                var cClass  = waitCtrl.className;
                // ______________________________________________________ Hide control ___
                waitCtrl.style.display = 'none';
                // __________________________ Simulate original control as a container ___
                o2jse.waitObj = o2jse.createEl(false, 'DIV', cClass);
                // _________________________________________ This is an insertAfter()! ___
                pNode.insertBefore(o2jse.waitObj, waitCtrl.nextSibling);
                var bgImg = (o2jse.waitObj.currentStyle ||
                             window.getComputedStyle(o2jse.waitObj,
                                                     false)).backgroundImage;
                if (bgImg.slice(0, 3).toLowerCase() == 'url') {
                    o2jse.waitObj.style.backgroundImage = 'none';
                    }
                o2jse.waitObj.style.width  = cWidth + 'px';
                o2jse.waitObj.style.height = cHeight + 'px';
                // ___________________________ Create wait image inside pseudo-control ___
                var wObj            = o2jse.createEl(o2jse.waitObj,
                                                     'DIV',
                                                     'jx_inctrl_wait',
                                                     '&nbsp;');
                wObj.style.position = 'relative';
                wObj.style.left     = parseInt((cWidth - wObj.offsetWidth) / 2) + 'px';
                wObj.style.top      = parseInt((cHeight - wObj.offsetHeight) / 2) + 'px';
                break;
            }
        }

    };


/**
 * Executes zoom action on activable controls
 *
 * @param object  eventObj   Event object when fired
 */
o2jse.ctrl.zoom = function(eventObj) {

    var stdEvent = o2jse.event.std(eventObj);
    var infObj   = stdEvent.target.o2;
    // __________________________________ Preserve waiting requests (submit-on-change) ___
    if (o2jse.submitting) {
        jxjs.cachedObj   = stdEvent.target;
        jxjs.cachedEvent = eventObj;
        jxjs.cachedCmd   = "if(jxjs.cachedObj.style.display != 'none' && " +
                              "jxjs.cachedObj.ondblclick){\n" +
                              "o2jse.submitting=false;\n" +
                              "o2jse.ctrl.zoom(jxjs.cachedEvent);}\n";
        return false;
        }
    if (stdEvent.target.blur) {
        stdEvent.target.blur();
        }
    if (infObj.z) {
        stdEvent.stop();
        o2jse.infoForm['o2lastform'].value = infObj.f;
        o2jse.infoForm['o2lastctrl'].value = infObj.c;
        var zoomCtrl                       = o2jse.createInput(o2jse.infoForm,
                                                               "hidden",
                                                               "",
                                                               infObj.c,
                                                               "jxzoom");
        zoomCtrl.o2                        = infObj;
        zoomCtrl.o2.fret                   = true;
        o2jse.ctrl.make_waiting(stdEvent.target);
        if (o2jse.cliMode) {
            jxjs.request();
            }
        else {
            o2jse.cmd.submit(infObj.e);
            }
        o2jse.removeEl(zoomCtrl);
        delete zoomCtrl;
        }
    return false;

    };


/**
 * Returns an array of active controls names by Janox type
 *
 * @param {String}  ctrlType      Janox control type to filter for
 * @param {Object}  rootElement   Top level element to start filtering from
 */
o2jse.ctrl.getByJxType = function(ctrlType, rootElement) {

    var retList = [];
    var retCtrl = null;
    var root    = rootElement || o2jse.infoForm;
    var exeId   = o2jse.infoForm['o2_prgexeid'].value;
    var found   = false;
    var walker  = document.createTreeWalker(document.body,
                                            NodeFilter.SHOW_ELEMENT,
                                            function(node) {
                                               if ((node.o2 ||
                                                    (node.hasAttribute &&
                                                     node.hasAttribute("o2") &&
                                                     o2jse.ctrl.init(node))) &&
                                                   node.o2.cT == ctrlType &&
                                                   node.o2.e == exeId) {
                                                  return NodeFilter.FILTER_ACCEPT;
                                                  }
                                               else {
                                                  return NodeFilter.FILTER_SKIP;
                                                  }
                                               },
                                            false);
    while (walker.nextNode()) {
        // ____________________________ Check other conditions - Visible, enabled, ... ___
        if (walker.currentNode.style.display != "none") {
            retCtrl = walker.currentNode.o2.c;
            found   = false;
            // _________________ Check if already in list (repeated elements in table) ___
            for (var i = retList.length - 1; i >= 0; i--) {
                if (retList[i] == retCtrl) {
                    found = true;
                    break;
                    }
                }
            if (!found) {
                retList[retList.length] = retCtrl;
                }
            }
        }
    return retList;

    };


/**
 * Call data-history for control
 *
 * @param object  targetObj   Target object when fired
 */
o2jse.ctrl.log = function(targetObj) {

    var infObj = targetObj.o2;
    if (infObj.log) {
        var logCtrl = o2jse.createInput(o2jse.infoForm,
                                        "hidden",
                                        "",
                                        infObj.c,
                                        "jxlogctrl");
        logCtrl.o2      = infObj;
        logCtrl.o2.fret = true;
        o2jse.cmd.submit(infObj.e);
        }
    return false;

    };


/**
 * Executes action on click on buttons, hrefs and clickable images
 *
 * @param object  targetObj   Event object when fired
 */
o2jse.ctrl.btnExe = function(targetObj) {

    o2jse.ctrl.init(targetObj);
    o2Info = targetObj.o2;
    // __________________________________ Preserve waiting requests (submit-on-change) ___
    if (o2jse.submitting) {
        jxjs.cachedObj = targetObj;
        jxjs.cachedCmd = "if(jxjs.cachedObj.style.display != 'none' && " +
                            "jxjs.cachedObj.onclick){\n" +
                            "o2jse.submitting=false;\n" +
                            "o2jse.ctrl.btnExe(jxjs.cachedObj);}\n";
        return false;
        }
    if (o2jse.pu.timer) {
        clearTimeout(o2jse.pu.timer);
        }
    if (o2Info.msg == "" || o2jse.cmd.confirm(o2Info.msg)) {
        o2jse.infoForm['o2_prgexeid'].value = o2Info.e;
        o2jse.infoForm['o2_action'].value   = o2Info.action;
        o2jse.infoForm['o2lastform'].value  = o2Info.f;
        o2jse.infoForm['o2lastctrl'].value  = o2Info.c;
        if (o2Info.fret) {
            o2jse.ctrl.make_waiting(targetObj);
            if (o2jse.cliMode) {
                jxjs.request(targetObj, targetObj.o2.action);
                }
            else {
                o2jse.cmd.submit(o2Info.e);
                }
            }
        else if (o2Info.cT == "button") {
            o2jse.ctrl.make_waiting(targetObj);
            }
        }

    };


/**
 * Execute selection in table
 *
 * @param object  sourceObj      Object firing selection change (storing view name)
 * @param integer newSelection   New selected row on view
 */
o2jse.ctrl.viewSelection  = function(sourceObj, newSelection) {

    o2jse.ctrl.init(sourceObj);
    var view_name = sourceObj.o2.v + "_vs" + sourceObj.o2.e;
    if (o2jse.infoForm[view_name]) {
        o2jse.infoForm[view_name].value = newSelection;
        }
    else {
        o2jse.createInput(o2jse.infoForm, "hidden", "", newSelection, view_name);
        }

    };


/**
 * Activates multipage control functionalities
 *
 * @param object   targetObj   Input HTML field related to multipage
 * @param booblean noRequest   Avoid submit/request - Used by jxjs.ctr to change pane
 */
o2jse.ctrl.multiPage = function(targetObj, noRequest) {

    o2jse.ctrl.init(targetObj);
    var o2Info    = targetObj.o2;
    var ctrlName  = o2Info.c + String(o2Info.e);
    var mpField   = o2jse.infoForm[ctrlName];
    var oldValue  = mpField.value;
    mpField.value = o2Info.page;
    o2jse.ctrl.init(mpField);
    // ____________________________________________________ Change page in client mode ___
    var objDivClose = document.getElementById(ctrlName + "_p" + parseInt(oldValue));
    var objBtnClose = document.getElementById(ctrlName + "_mpb" + parseInt(oldValue));
    var objDivOpen  = document.getElementById(ctrlName + "_p" + parseInt(o2Info.page));
    var objBtnOpen  = document.getElementById(ctrlName + "_mpb" + parseInt(o2Info.page));
    objDivClose.style.display = 'none';
    objBtnClose.className     = mpField.o2.cssoff;
    objBtnClose.tabIndex      = "0";
    objBtnClose.onclick       = function() { o2jse.ctrl.multiPage(this); };
    objDivOpen.style.display  = 'block';
    objBtnOpen.className      = mpField.o2.csson;
    objBtnOpen.tabIndex       = "-1";
    objBtnOpen.onclick        = "";
    o2jse.infoForm['o2lastform'].value = o2Info.f;
    o2jse.infoForm['o2lastctrl'].value = o2Info.c;
    // ____________________________________________________________ Display wait image ___
    if (objDivOpen.innerHTML.trim() == "") {
        objDivOpen.innerHTML = "<div class='jx_inctrl_wait'>&nbsp;</div>";
        }
    if (!noRequest) {
        // ______________________________________________________ Execute AJAX request ___
        if (o2jse.cliMode) {
            jxjs.request(mpField, mpField.value);
            }
        // ____________________________________________________________ Execute submit ___
        else {
            o2jse.cmd.ctrlUpd(mpField);
            }
        }

    };


/**
 * DB-grid control functions collection
 *
 */
o2jse.tab = {

    gridPlus         : false,
    autoSaveSettings : true,
    gridMultiDel     : true,
    dblClickAct      : "",
    wheelPaging      : true,
    sortList         : []

    };


/**
 * Handler for OnClick events on table rows
 *
 * @param object targetObj   Object on which event is fired
 * @param object eventObj    Event object fired
 */
o2jse.tab.select = function(targetObj, eventObj) {

    if (o2jse.cliMode) {
        var tg = o2jse.event.std(eventObj).target;
        o2jse.ctrl.init(tg);
        var sourceRow = null;
        var ctrl      = null;
        // _________________________________________________________ Click on TD space ___
        if (tg.tagName == "TD" && tg.firstChild) {
            tg = tg.firstChild;
            o2jse.ctrl.init(tg);
            }
        // __________________________________________________________ Click on control ___
        if (tg.o2.cT == "edit") {
            ctrl = o2jse.infoForm[tg.o2.c + tg.o2.e];
            }
        // __________________________________________________________ Click on control ___
        else if (tg.o2.cT == "listcombo") {
            ctrl = o2jse.infoForm[tg.o2.c + tg.o2.e + "_desc"];
            }
        // ___________________________________________________________ Seek source row ___
        if (ctrl) {
            var tdP = ctrl.parentNode;
            if (tdP) {
                var trP = tdP.parentNode;
                if (trP && trP.tagName == "TR") {
                    sourceRow = (trP.parentNode.lastRow ?
                                 trP.parentNode.lastRow : trP);
                    }
                }
            if (ctrl.disabled || ctrl.readOnly) {
                ctrl = null;
                }
            else {
                o2jse.infoForm['o2lastform'].value = tg.o2.f;
                o2jse.infoForm['o2lastctrl'].value = tg.o2.c;
                }
            }
        o2jse.tab.moveSel(sourceRow, targetObj);
        // ____________ Pinned columns are present and method called on pinned columns ___
        var pseudoC = document.getElementById(tg.o2.c + tg.o2.e + '_pseudoC_tab');
        if (pseudoC) {
            var srcTab = document.getElementById(tg.o2.c + tg.o2.e + '_tab');
            // ___ Method called on pinned columns: move selection on standard columns ___
            if (pseudoC.contains(tg)) {
                if (sourceRow) {
                    var sRow   = sourceRow.rowIndex +
                                 (srcTab.getElementsByTagName('TH') ? tg.o2.lines : 0);
                    sourceRow  = srcTab.rows[sRow];
                    }
                var tRow   = targetObj.rowIndex +
                             (srcTab.getElementsByTagName('TH') ? tg.o2.lines : 0);
                targetRow  = srcTab.rows[tRow];
                o2jse.tab.moveSel(sourceRow, targetRow, true);
                }
            // ___ Method called on standard columns: move selection on pinned columns ___
            else {
                if (sourceRow) {
                    var sRow   = sourceRow.rowIndex -
                                 (srcTab.getElementsByTagName('TH') ? tg.o2.lines : 0);
                    sourceRow  = pseudoC.rows[sRow];
                    }
                var tRow   = targetObj.rowIndex -
                             (srcTab.getElementsByTagName('TH') ? tg.o2.lines : 0);
                targetRow  = pseudoC.rows[tRow];
                o2jse.tab.moveSel(sourceRow, targetRow, true);
                }
            }
        // _______________________________________________ Set focus to target control ___
        if (ctrl && ctrl.focus) {
            ctrl.focus();
            if (ctrl.select) {
                ctrl.select();
                }
            }
        }
    else {
        o2jse.ctrl.init(targetObj);
        var objInfo = targetObj.o2;
        // ______________ Input object storing selection value is named like the table ___
        var rowNo = parseInt(targetObj.rowIndex / objInfo.lines);
        o2jse.infoForm[objInfo.c + objInfo.e].value = rowNo;
        o2jse.cmd.ctrlUpd(targetObj);
        }

    };


/**
 * Handler for OnClick events on table column headers (sorting button)
 *
 * @param object targetObj   Object on which event is fired
 */
o2jse.tab.sort = function(targetObj) {

    o2jse.ctrl.init(targetObj);
    var sortField = o2jse.createInput(o2jse.infoForm,
                                      "hidden",
                                      "",
                                      targetObj.o2.inctrl,
                                      targetObj.o2.c + "_jxsort" + targetObj.o2.e);
    sortField.o2  = targetObj.o2;
    o2jse.cmd.ctrlUpd(sortField);

    };


/**
 * Handler for OnMouseOver events on table rows
 *
 * @param object targetObj   HTML element on which event is fired
 * @param object eventObj    Event object passed by event
 * @param string extraCss    Class name to set on element
 */
o2jse.tab.rowOn = function(targetObj, eventObj, extraCss) {

    o2jse.ctrl.init(targetObj);
    var newClass = targetObj.o2.cssp + (extraCss ? ' ' + extraCss : '');
    var pseudoC  = document.getElementById(targetObj.o2.c + targetObj.o2.e +
                                           '_pseudoC_tab');
    // ____________________________________________________ Pinned columns are present ___
    if (pseudoC) {
        // _______________________________________ Method colled on pinned columns row ___
        if (pseudoC.contains(targetObj)) {
            var srcTab = document.getElementById(targetObj.o2.c + targetObj.o2.e + '_tab');
            var row    = targetObj.rowIndex +
                         (srcTab.getElementsByTagName('TH') ? targetObj.o2.lines : 0);
            srcTab.rows[row].className = newClass;
            }
        // _____________________________________________ Method colled on standard row ___
        else {
            var row = targetObj.rowIndex -
                      (targetObj.parentNode.parentNode.getElementsByTagName('TH') ?
                       targetObj.o2.lines : 0);
            pseudoC.rows[row].className = newClass;
            }
        }
    targetObj.className = newClass;

    };


/**
 * Handler for OnMouseOut events on table rows
 *
 * @param object targetObj   HTML element on which event is fired
 * @param object eventObj    Event object passed by event
 * @param string extraCss    Class name to set on element
 */
o2jse.tab.rowOut = function(targetObj, eventObj, extraCss) {

    o2jse.ctrl.init(targetObj);
    var newClass = targetObj.o2.cssc + (extraCss ? " " + extraCss : "");
    var pseudoC  = document.getElementById(targetObj.o2.c + targetObj.o2.e +
                                           '_pseudoC_tab');
    // ____________________________________________________ Pinned columns are present ___
    if (pseudoC) {
        // _______________________________________ Method colled on pinned columns row ___
        if (pseudoC.contains(targetObj)) {
            var srcTab = document.getElementById(targetObj.o2.c + targetObj.o2.e + '_tab');
            var row    = targetObj.rowIndex +
                         (srcTab.getElementsByTagName('TH') ? targetObj.o2.lines : 0);
            srcTab.rows[row].className = newClass;
            }
        // _____________________________________________ Method colled on standard row ___
        else {
            var row = targetObj.rowIndex -
                      (targetObj.parentNode.parentNode.getElementsByTagName('TH') ?
                       targetObj.o2.lines : 0);
            pseudoC.rows[row].className = newClass;
            }
        }
    targetObj.className = newClass;

    };


/**
 * Handler for click events on current read-only table row
 *
 * @param object targetObj   Row marker object to set focus to
 */
o2jse.tab.activateMarker = function(markerObj) {

    o2jse.ctrl.init(markerObj);
    var objInfo = markerObj.o2;
    o2jse.infoForm['o2lastform'].value = objInfo.f;
    o2jse.infoForm['o2lastctrl'].value = objInfo.c;
    markerObj.focus();

    };


/**
 * Execute table client selection by keyboard
 *
 * @param boolean bckwd     If TRUE down-up direction else up-down
 * @param object  ctrlObj   Marker object on which event is fired
 */
o2jse.tab.selectRow = function(bckwd, ctrlObj) {

    bckwd        = bckwd != null && bckwd;
    var oldRow   = ctrlObj.parentNode.parentNode;
    var oldRowId = oldRow.rowIndex;
    var newRowId = (bckwd ? (oldRowId - 1) : (oldRowId + 1));
    var newRow   = ctrlObj.parentNode.parentNode.parentNode.rows[newRowId - 1];
    var o2Data   = ctrlObj.o2;
    var seleField;
    if (newRowId > 0 && newRowId <= o2Data.rows) {
        var navName = o2Data.nav + o2Data.e + "_o2sel";
        if (o2jse.infoForm[navName]) {
            o2jse.infoForm[navName].value = newRowId;
            }
        else {
            seleField    = o2jse.createInput(o2jse.infoForm,
                                             "hidden",
                                             "",
                                             newRowId,
                                             navName);
            seleField.o2 = o2Data;
            }
        // ___________________________________________________________ Move row marker ___
        var emptyT       = document.createElement("SPAN");
        emptyT.innerHTML = "&nbsp;";
        newRow.cells[0].replaceChild(oldRow.cells[0].
                                      replaceChild(emptyT, oldRow.cells[0].childNodes[0]),
                                     newRow.cells[0].childNodes[0]);
        }
    o2jse.infoForm['o2lastform'].value = o2Data.f;
    o2jse.infoForm['o2lastctrl'].value = o2Data.c;
    ctrlObj.focus();

    };


/**
 * Executes DoubleClick actions on table rows
 *
 * @param {Object} eventObj   DblClick event object passed by TD element
 */
o2jse.tab.rowDblClick = function(eventObj) {

    // _________________________________________________ No action selected in options ___
    if (o2jse.tab.dblClickAct.trim() == "") {
        return;
        }
    var stdEvent  = o2jse.event.std(eventObj);
    var targetObj = stdEvent.target;
    o2jse.ctrl.init(targetObj);
    var infoObj = targetObj.o2;
    var navName = "";
    // _________________________________________________ Get related navigator control ___
    if (infoObj.nav) {
        navName = infoObj.nav + String(infoObj.e);
        }
    // _____________________________________ Fire selected action on navigator control ___
    if (navName) {
        stdEvent.stop();
        var navLocal = o2jse.infoForm[navName];
        o2jse.ctrl.init(navLocal);
        // _____________________________________________________________ Select action ___
        if (navLocal.o2.navSele &&
            (o2jse.tab.dblClickAct == "S" || o2jse.tab.dblClickAct == "B")) {
            o2jse.nav.btnExe(document.getElementById(navName + "_selebtn"));
            }
        // _____________________________________________________________ Detail action ___
        else if (navLocal.o2.navDet &&
                 (o2jse.tab.dblClickAct == "D" || o2jse.tab.dblClickAct == "B")) {
            o2jse.nav.btnExe(document.getElementById(navName + "_detbtn"));
            }
        }

    };


/**
 * Starts time-out for action of table column moving
 *
 * @param object eventObj    On click event object
 */
o2jse.tab.startColMovingTO = function(eventObj) {

    o2jse.tab.colMovTimeOut = setTimeout(function() {
                                             o2jse.tab.startColMoving(eventObj);
                                             }, 700);
    document.onmouseup = function() { if (o2jse.tab.colMovTimeOut) {
                                          clearTimeout(o2jse.tab.colMovTimeOut);
                                          };
                                      document.onmouseup = null;
                                      };

    };


/**
 * Starts action of table column moving
 *
 * @param object eventObj    On click event object
 */
o2jse.tab.startColMoving = function(eventObj) {

    var stdEvent  = o2jse.event.std(eventObj);
    var targetObj = eventObj.target;
    o2jse.ctrl.init(targetObj);
    if (stdEvent.button != 2) {
        stdEvent.stop();
        var colObj = targetObj;
        var tabObj = document.getElementById(colObj.o2.c + colObj.o2.e +"_tab");
        var divObj = tabObj.parentNode;
        var ths    = tabObj.getElementsByTagName("thead")[0].getElementsByTagName("th");
        var hObj   = colObj;
        var totY   = 0;
        do {
            totY += hObj.offsetTop;
            } while (hObj = hObj.offsetParent);
        // ________________________________________________________ Calculate column X ___
        var wObj = colObj;
        var totX = 0;
        do {
            totX += wObj.offsetLeft;
            } while (wObj = wObj.offsetParent);
        // _________________________________________________________ Calculate table X ___
        wObj = divObj;
        var tabX = 0;
        do {
            tabX += wObj.offsetLeft;
            } while (wObj = wObj.offsetParent);
        var sLine            = o2jse.createEl(o2jse.elBody, "div", "o2_tab_resize_line");
        sLine.style.position = "absolute";
        sLine.style.height   = divObj.parentNode.offsetHeight + "px";
        sLine.style.top      = totY + "px";
        sLine.style.left     = totX + "px";
        o2jse.tab.movingCol  = { t: tabObj,
                                 s: colObj.o2.inctrl,
                                 c: ths,
                                 o: colObj.o2.col,
                                 m: sLine,
                                 l: tabX,
                                 r: tabX + divObj.offsetWidth };
        o2jse.cmd.opacity(tabObj, 50);
        // ____________________________________ Make a glass stoppable by pressing ESC ___
        o2jse.cmd.startGlass(o2jse.tab.moveColTo,
                             o2jse.tab.stopColMoving,
                             "move",
                             false,
                             true);

        }

    };


/**
 * Executes action of table column moving
 *
 * @param object eventObj   Mouse move event object
 */
o2jse.tab.moveColTo = function(eventObj) {

    var movingCol = o2jse.tab.movingCol;
    var stdEvent  = o2jse.event.std(eventObj);
    stdEvent.stop();
    var x = stdEvent.x;
    var rect;
    var lBorder;
    if (x >= movingCol.l && x <= movingCol.r) {
        for (var i = 0; i < movingCol.c.length; i++) {
            rect = movingCol.c[i].getBoundingClientRect();
            if (x >= rect.left && x <= rect.right) {
                lBorder = rect.left;
                }
            }
        movingCol.m.style.left = lBorder + 'px';
        }
    // ___________________________________________ Prevent text selection while moving ___
    if (document.selection) {
        document.selection.empty()
        }
    else {
        window.getSelection().removeAllRanges()
        }

    };


/**
 * Stops action of table column moving
 *
 * @param {Object}  eventObj   Mouse up event object
 * @param {Boolean} stopped    If action has been stopped pressing ESC
 */
o2jse.tab.stopColMoving = function(eventObj, stopped) {

    var movingCol = o2jse.tab.movingCol;
    var stdEvent  = o2jse.event.std(eventObj);
    stdEvent.stop();
    o2jse.cmd.stopGlass();
    o2jse.cmd.opacity(movingCol.t, 100);
    o2jse.removeEl(movingCol.m);
    if (stopped) {
        return;
        }
    var tab = movingCol.t;
    o2jse.ctrl.init(tab);
    var x = stdEvent.x;
    var rect;
    var colIdx;
    if (x >= movingCol.l && x <= movingCol.r) {
        for (var i = 0; i < movingCol.c.length; i++) {
            rect = movingCol.c[i].getBoundingClientRect();
            if (x >= rect.left && x <= rect.right) {
                o2jse.ctrl.init(movingCol.c[i]);
                colIdx = movingCol.c[i].o2.col;
                }
            }
        }
    if (!colIdx) {
        colIdx = 1;
        }
    else if (colIdx > movingCol.o) {
        colIdx--;
        }
    var sortField = o2jse.createInput(o2jse.infoForm,
                                      "hidden",
                                      "",
                                      "array('" + movingCol.s + "'=>" + colIdx + ")",
                                      tab.o2.c + "_jxocol" + tab.o2.e);
    sortField.o2  = tab.o2;
    if (o2jse.cliMode) {
        jxjs.request(sortField, sortField.value);
        }
    else {
        o2jse.cmd.ctrlUpd(sortField);
        }
    o2jse.removeEl(sortField);

    };


/**
 * Starts action of table column sizing
 *
 * @param object eventObj    On click event object
 */
o2jse.tab.startSizing = function(eventObj) {

    var stdEvent  = o2jse.event.std(eventObj);
    var targetObj = eventObj.target;
    o2jse.ctrl.init(targetObj);
    if (stdEvent.button != 2) {
        stdEvent.stop();
        var tabObj = document.getElementById(targetObj.o2.c + targetObj.o2.e +"_tab");
        var divObj = tabObj.parentNode;
        var ths    = tabObj.getElementsByTagName("thead")[0].getElementsByTagName("th");
        var colIdx = 0;
        var colObj;
        do {
            colIdx++;
            colObj = ths[targetObj.o2.col - colIdx];
            } while (colObj.style.display == "none");
        var sLine  = o2jse.createEl(o2jse.elBody, "div", "o2_tab_resize_line");
        var hObj   = targetObj;
        var totY   = 0;
        do {
            totY += hObj.offsetTop;
            } while (hObj = hObj.offsetParent);
        o2jse.ctrl.init(colObj);
        sLine.style.position = "absolute";
        sLine.style.height   = tabObj.getElementsByTagName("tbody")[0].offsetHeight +
                               targetObj.parentNode.offsetHeight + "px";
        sLine.style.top      = totY + "px";
        sLine.style.left     = stdEvent.x + "px";
        o2jse.tab.movingCol  = { t: tabObj, c: colObj.o2.inctrl, m: sLine,
                                 s: stdEvent.x, l: stdEvent.x - colObj.offsetWidth,
                                 r: divObj.offsetWidth + stdEvent.x};
        o2jse.cmd.opacity(tabObj, 50);
        o2jse.cmd.startGlass(o2jse.tab.sizeTo, o2jse.tab.stopSizing, "col-resize");
        }

    };


/**
 * Executes action of table column sizing
 *
 * @param object eventObj   Mouse move event object
 */
o2jse.tab.sizeTo = function(eventObj) {

    var movingCol = o2jse.tab.movingCol;
    var stdEvent  = o2jse.event.std(eventObj);
    stdEvent.stop();
    if (stdEvent.x >= movingCol.l && stdEvent.x <= movingCol.r) {
        movingCol.m.style.left = stdEvent.x + 'px';
        }

    };


/**
 * Stops action of table column sizing
 *
 * @param object eventObj   Mouse up event object
 */
o2jse.tab.stopSizing = function(eventObj) {

    var movingCol = o2jse.tab.movingCol;
    var stdEvent  = o2jse.event.std(eventObj);
    stdEvent.stop();
    o2jse.cmd.stopGlass();
    o2jse.cmd.opacity(movingCol.t, 100);
    o2jse.removeEl(movingCol.m);

    var targetObj = movingCol.t;
    o2jse.ctrl.init(targetObj);
    var sizeField = o2jse.createInput(o2jse.infoForm,
                                      "hidden",
                                      "",
                                      movingCol.c + ":" + (stdEvent.x - movingCol.s),
                                      targetObj.o2.c + "_jxsize" + targetObj.o2.e);
    sizeField.o2  = targetObj.o2;
    if (o2jse.cliMode) {
        jxjs.request(sizeField, sizeField.value);
        }
    else {
        o2jse.cmd.ctrlUpd(sizeField);
        }
    o2jse.removeEl(sizeField);

    };


/**
 * Changes table row in CLIENT MODE
 *
 * @param object targetObj   Object on which event is fired
 */
o2jse.tab.changeRow = function(targetObj, movDir, navName) {

    o2jse.ctrl.init(targetObj);
    // _______________________________ If event is fired on buttons of unselected rows ___
    if (targetObj.o2.cT == "button" && !targetObj.o2.fret) {
        return false;
        }
    targetObj.blur();
    var sourceRow = targetObj.parentNode.parentNode;
    var targetRow;
    // _________________________________________________________ Go to previous record ___
    if (movDir == "P") {
        if (sourceRow.rowIndex > 1) {
            targetRow = sourceRow.previousSibling;
            }
        // _________________________________________________________________ First row ___
        else {
            if (navName) {
                var navLocal = o2jse.infoForm[navName];
                o2jse.ctrl.init(navLocal);
                if (navLocal.o2.navNav &&
                    (navBtn = document.getElementById(navName + "_pbtn"))) {
                    return o2jse.nav.btnExe(navBtn);
                    }
                }
            }
        }
    // _____________________________________________________________ Go to next record ___
    else {
        if (sourceRow.rowIndex < sourceRow.parentNode.rows.length) {
            targetRow = sourceRow.nextSibling;
            }
        // __________________________________________________________________ Last row ___
        else {
            if (navName) {
                var navLocal = o2jse.infoForm[navName];
                o2jse.ctrl.init(navLocal);
                if (navLocal.o2.navNav &&
                    (navBtn = document.getElementById(navName + "_nbtn"))) {
                    return o2jse.nav.btnExe(navBtn);
                    }
                }
            }
        }
    if (targetRow) {
        o2jse.ctrl.init(targetRow);
        // _________________________________________________ Client selection in table ___
        o2jse.tab.moveSel(sourceRow, targetRow);
        }
    // ___________________________________________________________________ Reset focus ___
    var targetInRow;
    if (targetObj.name) {
        var targetsInRow = document.getElementsByName(targetObj.name);
        if (targetsInRow.length > 1) {
            targetInRow = targetsInRow[targetRow.rowIndex - 1];
            }
        else {
            targetInRow = targetObj;
            }
        }
    else {
        targetInRow = targetObj;
        }
    if (targetInRow.focus) {
        targetInRow.focus();
        if (targetInRow.select) {
            targetInRow.select();
            }
        }

    };


/**
 * Change table selection from sourceRow to targetRow in CLIENT MODE.
 * (For selection by keyboard on row-marker see o2jse.tab.selectRow() method)
 *
 * @param object  sourceRow   Coming-from row (actually selected - contains fields)
 * @param object  targetRow   Going-to row (actually unselected - contains text id DIVs)
 * @param noolean noRequest   Avoid request execution, used by pinned columns
 */
o2jse.tab.moveSel = function(sourceRow, targetRow, noRequest) {

    o2jse.ctrl.init(targetRow);
    var rowsCont  = targetRow.parentNode;
    var viewField = o2jse.infoForm[targetRow.o2.v + "_vs" + targetRow.o2.e];
    // ________________________________________________ Retrieves current selected row ___
    if (!sourceRow) {
        if (rowsCont.lastRow) {
            sourceRow = rowsCont.lastRow;
            }
        else {
            sourceRow = rowsCont.rows[viewField.value - 1];
            o2jse.ctrl.init(sourceRow);
            }
        }
    rowsCont.lastRow = targetRow;
    // _____________________________________________________________ Same row, no move ___
    if (sourceRow == targetRow) {
        return;
        }
    // __________________________________________ Comunicates changed fields to server ___
    var newSele = parseInt(targetRow.rowIndex / targetRow.o2.lines);
    if (!noRequest) {
        jxjs.request(targetRow, newSele);
        }
    // ____________________________________ Set new view selection for the environment ___
    viewField.value = newSele;
    // __________________________________________ Loop on cells for translating fields ___
    for (var cellId = (sourceRow.cells.length - 1); cellId >= 0; cellId--) {
        var sourceField = o2jse.firstRealChild(sourceRow.cells[cellId]);
        var targetField = o2jse.firstRealChild(targetRow.cells[cellId]);
        if (targetField != null) {
            // ____________________________________ Translating single cell properties ___
            o2jse.ctrl.init(sourceField);
            o2jse.ctrl.init(targetField);
            var sourceParent = sourceField.parentNode;
            var targetParent = targetField.parentNode;
            var bodyClass    = sourceParent.getAttribute("cssc");
            var pointClass   = targetParent.getAttribute("cssp");
            sourceParent.removeAttribute("cssc");
            targetParent.removeAttribute("cssp");
            targetParent.setAttribute("cssc", targetParent.className);
            sourceParent.setAttribute("cssp", sourceParent.className);
            targetParent.className = pointClass;
            sourceParent.className = bodyClass;
            // _____________________________________ Popup on column of disabled table ___
            if (sourceParent.onmouseover) {
                targetParent.onmouseover = sourceParent.onmouseover;
                targetParent.onmouseout  = sourceParent.onmouseout;
                sourceParent.onmouseover = null;
                sourceParent.onmouseout  = null;
                }
            var rowMarker = (sourceField.className.substr(0, 13) == "o2_tab_marker");
            // _____________________________________ Edit table & buttons & row-marker ___
            if (sourceField.tagName.toLowerCase() != "div" ||
                sourceField.o2                             ||
                rowMarker) {
                switch (sourceField.o2.cT) {
                    case "img":
                        sourceField.o2.fret = false;
                        targetField.o2.fret = true;
                        break;
                    case "button":
                        sourceField.o2.fret = false;
                        targetField.o2.fret = true;
                        targetField.id      = sourceField.id;
                        sourceField.removeAttribute('id');
                        break;
                    case "listcombo":
                        var targetClone = targetField.cloneNode(true);
                        // __________________________________________________ List-box ___
                        if (sourceField.options) {
                            var sourceSele        = sourceField.options.selectedIndex;
                            targetClone.innerHTML = sourceField.options[sourceSele].text;
                            sourceField.value     = "";
                            sourceField.value     = targetField.innerHTML.trim().
                                                    substr(0, sourceField.o2.s);
                            targetParent.replaceChild(sourceField, targetField);
                            sourceParent.appendChild(targetClone);
                            }
                        // _____________________________________________ Combo look-up ___
                        else {
                            // _____________________ Move hidden field with code value ___
                            $code_field = document.getElementsByName(targetField.o2.c +
                                                                     targetField.o2.e);
                            if ($code_field[0]) {
                                targetParent.appendChild($code_field[0]);
                                }
                            // ______________________________________________ Disabled ___
                            if (sourceField.tagName.toLowerCase() == 'div') {
                                targetClone.innerHTML = sourceField.innerHTML;
                                sourceField.innerHTML = targetField.innerHTML;
                                targetParent.replaceChild(sourceField, targetField);
                                sourceParent.innerHTML = "";
                                sourceParent.appendChild(targetClone);
                                sourceField.dropDownActive = false;
                                delete sourceField.saveValue;
                                sourceParent.insertBefore(targetClone,
                                                          sourceParent.firstChild);
                                }
                            // _______________________________________________ Enabled ___
                            else {
                                targetClone.innerHTML      = sourceField.value;
                                var newValue               = targetField.innerHTML.trim();
                                sourceField.value          = newValue.decode();
                                sourceField.dropDownActive = false;
                                delete sourceField.saveValue;
                                targetParent.replaceChild(sourceField, targetField);
                                sourceParent.insertBefore(targetClone,
                                                          sourceParent.firstChild);
                                }
                            }
                        break;
                    // _____________________________________________________ Check-box ___
                    case "checkbox":
                        var targetClone     = targetField.cloneNode(true);
                        targetClone.checked = sourceField.checked;
                        sourceField.checked = targetField.checked;
                        if (targetClone.hasAttribute('name')) {
                            targetClone.removeAttribute('name');
                            }
                        sourceField.setAttribute('name',
                                                 sourceField.o2.c + sourceField.o2.e);
                        targetParent.replaceChild(sourceField, targetField);
                        sourceParent.innerHTML = "";
                        sourceParent.appendChild(targetClone);
                        break;
                    default:
                        // ________________________________________________ Row-marker ___
                        if (rowMarker) {
                            var targetClone       = targetField.cloneNode(true);
                            targetClone.innerHTML = sourceField.innerHTML;
                            sourceField.innerHTML = targetField.innerHTML;
                            targetParent.replaceChild(sourceField, targetField);
                            sourceParent.innerHTML = "";
                            sourceParent.appendChild(targetClone);
                            if (sourceField.focus) {
                                sourceField.focus();
                                }
                            }
                        // ______________________ Standard fields like edit, text, ... ___
                        else if (typeof sourceField.value != "undefined") {
                            var targetClone       = targetField.cloneNode(true);
                            targetClone.innerHTML = sourceField.value;
                            var newValue          = targetField.innerHTML;
                            sourceField.value     = (newValue.trim() == "&nbsp;" ? "" :
                                                     newValue.decode());
                            targetParent.replaceChild(sourceField, targetField);
                            sourceParent.innerHTML = "";
                            sourceParent.appendChild(targetClone);
                            }
                        // ___________________________________________ Disabled fields ___
                        else if(sourceField.o2.cT == 'edit' ||
                                sourceField.o2.cT == 'text') {
                            var targetClone       = targetField.cloneNode(true);
                            targetClone.innerHTML = sourceField.innerHTML;
                            sourceField.innerHTML = targetField.innerHTML;
                            targetParent.replaceChild(sourceField, targetField);
                            sourceParent.innerHTML = "";
                            sourceParent.appendChild(targetClone);
                            }
                        else if (sourceField.id) {
                            targetField.id = sourceField.id;
                            sourceField.removeAttribute('id');
                            }
                        break;
                    }
                }
            // ______________________________________________ DIV has ID attribute set ___
            else if (sourceField.id) {
                targetField.id = sourceField.id;
                sourceField.removeAttribute('id');
                }
            }
        }
    // ____________________________________________________ Translating row properties ___
    o2jse.ctrl.init(sourceRow);
    var infoForm          = o2jse.infoForm;
    var focusCtrl         = infoForm[infoForm['o2lastctrl'].value +
                                     infoForm['o2_prgexeid'].value];
    var sourceSaveOver    = sourceRow.onmouseover;
    var sourceSaveOut     = sourceRow.onmouseout;
    var sourceSaveClick   = sourceRow.onclick;
    sourceRow.className   = sourceRow.o2.cssc;
    sourceRow.o2.tabRow   = true;
    sourceRow.onmouseover = targetRow.onmouseover;
    sourceRow.onmouseout  = targetRow.onmouseout;
    sourceRow.onclick     = targetRow.onclick;
    targetRow.className   = targetRow.o2.cssc + " o2tab_current";
    targetRow.onmouseover = sourceSaveOver;
    targetRow.onmouseout  = sourceSaveOut;
    targetRow.onclick     = sourceSaveClick;
    targetRow.o2.tabRow   = false;
    // ___________________________________________________ Update view selection value ___
    o2jse.ctrl.viewSelection(sourceRow, targetRow.rowIndex);
    if (focusCtrl && focusCtrl.focus) {
        focusCtrl.focus();
        if (focusCtrl.select) {
            focusCtrl.select();
            }
        }

    };


/**
 * Client-side table adjustament for horizontal and vertical scrolling, blank rows to fill
 * to fit and focus
 *
 * @param {String}  tabName      Name of the table to adjust
 * @param {Integer} fillRows     Number of blank rows to add
 * @param {Integer} scrollLeft   Number of pixel to scroll table from left
 * @param {Integer} scrollTop    Number of pixel to scroll table from top
 * @param {String}  focusCtrl    Name of control to give focus to
 */
o2jse.tab.set = function(tabName,
                         fillRows,
                         scrollLeft,
                         scrollTop,
                         focusCtrl) {

    var myTab        = document.getElementById(tabName + "_tab");
    var myCont       = myTab.parentNode;
    var contAll      = myCont.parentNode;
    var hScroll      = myCont.scrollWidth > myCont.clientWidth;
    var vScroll      = myCont.scrollHeight > myCont.clientHeight;
    var myHead       = myTab.getElementsByTagName("thead")[0];
    var myFoot       = myTab.getElementsByTagName("tfoot")[0];
    var hScrollName  = tabName + "_hscroll";
    var vScrollName  = tabName + "_vscroll";
    // __________________________________ Check table visibility and execute only once ___
    if (!myCont.offsetWidth || myTab.setDone) {
        return;
        }
    else {
        myTab.setDone = true;
        }
    // ________________________ Manage horizontal scrolling (always for client sizing) ___
    o2jse.createInput(contAll, "hidden", null, scrollLeft, hScrollName);
    if (scrollLeft > 0) {
        myCont.scrollLeft = scrollLeft;
        }
    // __________________________________________ Code to save horizontal scroll value ___
    codeOnScroll = "document.forms.o2form['" + hScrollName + "'].value=this.scrollLeft;";
    // _____________________________________________________ Manage vertical scrolling ___
    if (vScroll) {
        o2jse.createInput(contAll, "hidden", null, scrollTop, vScrollName);
        if (scrollTop > 0) {
            myCont.scrollTop = scrollTop;
            }
        // ________________________________________ Code to save vertical scroll value ___
        codeOnScroll+= "document.forms.o2form['" + vScrollName +
                       "'].value=this.scrollTop;";
        }
    // __________________________________________ Add pseudo-header if header in table ___
    if (myHead) {
        // _______________________ Prepare header (always done, useful for footer too) ___
        for (var i = (myHead.rows[0].cells.length - 1); i >= 0; i--) {
            var myCell = myHead.rows[0].cells[i];
            if (!myCell.style.width) {
                myCell.style.width = myCell.offsetWidth + "px";
                }
            }
        myTab.style.tableLayout = "fixed";
        // _________________________ If vertical scrolling needed create pseudo-header ___
        if (vScroll) {
            var hTab         = myTab.cloneNode(false);
            hTab.style.width = myTab.offsetWidth + "px";
            hTab.appendChild(myHead.cloneNode(true));
            // _______________________________________________ Add header to fixed div ___
            var pseudoH            = document.createElement("div");
            pseudoH.id             = tabName + "_pseudoH";
            pseudoH.style.position = "absolute";
            pseudoH.style.overflow = "hidden";
            pseudoH.style.top      = myCont.clientTop + "px";
            pseudoH.style.left     = myCont.clientLeft + "px";
            pseudoH.style.width    = myCont.clientWidth + "px";
            pseudoH.style.height   = myHead.offsetHeight + "px";
            pseudoH.appendChild(hTab);
            contAll.appendChild(pseudoH);
            // _____________________________ Code to scroll pseudo-header with content ___
            codeOnScroll+= "document.getElementById('" + pseudoH.id +
                           "').scrollLeft=this.scrollLeft;";
            }
        }
    // __________________________________________ Add pseudo-footer if footer in table ___
    if (myFoot && (vScroll || fillRows > 0)) {
        // ____________________________________________________________ Prepare footer ___
        for (var i = (myFoot.rows[0].cells.length - 1); i >= 0; i--) {
            var myCell         = myFoot.rows[0].cells[i];
            myCell.style.width = myCell.offsetWidth + "px";
            }
        myTab.style.tableLayout = "fixed";
        var fTab                = myTab.cloneNode(false);
        fTab.style.width        = myTab.offsetWidth + "px";
        fTab.appendChild(myFoot.cloneNode(true));
        // ___________________________________________________ Add footer to fixed div ___
        var pseudoF             = document.createElement("div");
        pseudoF.id              = tabName + "_pseudoF";
        pseudoF.style.position  = "absolute";
        pseudoF.style.overflow  = "hidden";
        pseudoF.style.bottom    = (myCont.offsetHeight -
                                   myCont.clientHeight -
                                   myCont.clientTop) + "px";
        pseudoF.style.left      = myCont.clientLeft + "px";
        pseudoF.style.width     = myCont.clientWidth + "px";
        pseudoF.style.height    = myFoot.offsetHeight + "px";
        myFoot.style.visibility = "hidden";
        pseudoF.appendChild(fTab);
        contAll.appendChild(pseudoF);
        // _________________________________ Code to scroll pseudo-footer with content ___
        codeOnScroll+= "document.getElementById('" + pseudoF.id +
                       "').scrollLeft=this.scrollLeft;";
        }
    o2jse.ctrl.init(myTab);
    // _________________________________________ Add pseudo-columns for pinned columns ___
    if (myTab.o2.pc) {
        var pinCols = myTab.o2.pc;
        // ______________________ If horizontal scrolling needed create pseudo-columns ___
        if (hScroll) {
            // _______________ If table marker column is visible add it to pinned ones ___
            if (myTab.getElementsByTagName('TR')[0]
                     .firstElementChild.className.substr(0, 13) == 'o2_tab_marker') {
                pinCols++;
                }
            var startRow            = (myHead ? 1 : 0);
            var startTop            = myCont.clientTop +
                                      (myHead ? myHead.offsetHeight : 0);
            var fWidth              = 0;
            myTab.style.tableLayout = "fixed";
            var fTab                = myTab.cloneNode(false);
            fTab.id                 = tabName + '_pseudoC_tab';
            // _______________________________________________________ Prepare columns ___
            for (var i = startRow; i < myTab.rows.length; i++) {
                myTab.rows[i].style.height = myTab.rows[i].offsetHeight + 'px';
//                var fStart  = i - startRow;
                var fRow    = fTab.appendChild(myTab.rows[i].cloneNode(false));
                for (var n = 0; n < pinCols; n++) {
                    var myCell = myTab.rows[i].cells[n];
                    if (!myCell.style.width) {
                        myCell.style.width = myCell.offsetWidth + "px";
                        }
                    // _____________________________ Sum cells width to get full width ___
                    if (i == startRow) {
                        fWidth+= myCell.offsetWidth;
                        }
                    var newCell = myCell.cloneNode(true);
                    newCell.style.width = myCell.offsetWidth + 'px';
                    fRow.appendChild(newCell);
                    delete(myCell);
                    }
                delete(fRow);
                }
            // _____________________________________ Create headers for pinned columns ___
            if (myHead) {
                var fH = document.createElement('table').appendChild(myHead.cloneNode(false));
                var fR = fH.appendChild(myHead.rows[0].cloneNode(false));
                for (var n = 0; n < pinCols; n++) {
                    var myC         = myHead.rows[0].cells[n];
                    myC.style.width = myC.offsetWidth + 'px';
                    fR.appendChild(myC.cloneNode(true));
                    }
                var pseudoCH            = document.createElement('div');
                pseudoCH.id             = tabName + '_pseudoCH';
                pseudoCH.style.position = 'absolute';
                pseudoCH.style.top      = myCont.clientTop + 'px';
                pseudoCH.style.left     = myCont.clientLeft + 'px';
                pseudoCH.style.width    = fWidth + 'px';
                fR.style.height         = myHead.offsetHeight + 'px';
                pseudoCH.appendChild(fH);
                contAll.appendChild(pseudoCH);
                }
//            fTab.style.height      = myTab.offsetHeight + 'px';
            // ______________________________________________ Add columns to fixed div ___
            var pseudoC               = document.createElement('div');
            pseudoC.id                = tabName + '_pseudoC';
            pseudoC.className         = 'o2_tab_pinned';
            pseudoC.style.position    = 'absolute';
            pseudoC.style.overflow    = 'hidden';
            pseudoC.style.top         = startTop + 'px';
            pseudoC.style.left        = myCont.clientLeft + 'px';
            pseudoC.style.width       = fWidth + 'px';
            pseudoC.style.height      = (myCont.clientHeight -
                                         (myHead ? myHead.offsetHeight : 0)) + "px";
            pseudoC.appendChild(fTab);
            contAll.appendChild(pseudoC);
            // ____________________________ Code to scroll pseudo-columns with content ___
            codeOnScroll+= "document.getElementById('" + pseudoC.id +
                           "').scrollTop=this.scrollTop;";
            }

        }

    // _________________________________________ Assign code to onScroll event handler ___
    myCont.onscroll = new Function("o2jse.lu.listOff();" + codeOnScroll);
    if (fillRows == 0) {
        myTab.style.height = myCont.clientHeight + "px";
        }
    // ____________________________________________________ Manage mouse scroll events ___
    if (o2jse.tab.wheelPaging) {
        myCont.addEventListener('mousewheel',
                                function(e) { o2jse.tab.mouseScroll(e, myCont); });
        myCont.addEventListener('DOMMouseScroll',
                                function(e) { o2jse.tab.mouseScroll(e, myCont); });
        }
    // ________________________________________________________ Focus control in table ___
    if (focusCtrl) {
        o2jse.cmd.focus(focusCtrl);
        }

    };


/**
 * Manage mouse scrolling for pages navigation
 *
 * @param wEvent     Event object
 * @param viewPort   Table scrollable viewport DIV
 */
o2jse.tab.mouseScroll = function(wEvent, viewPort) {

    var stdEvent = o2jse.event.std(wEvent);
    var now      = (new Date).valueOf();
    // ____________________________________________________ If mouse wheel is scrolled ___
    if (stdEvent.delta) {
        var navName = stdEvent.target.o2.nav + String(stdEvent.target.o2.e);
        var doIt    = false;
        // _______________________________________________________________ Scroll down ___
        if (stdEvent.delta > 0) {
            // _________________________________________________ Scrollable space down ___
            if ((viewPort.scrollHeight - viewPort.scrollTop) <= viewPort.offsetHeight) {
                if (navBtn = document.getElementById(navName + "_npbtn")) {
                    if ((now - viewPort.scrollUpReady) > 300 ||
                        viewPort.scrollHeight <= viewPort.offsetHeight) {
                        doIt                   = true;
                        viewPort.scrollUpReady = 0;
                        }
                    else {
                        viewPort.scrollUpReady = now;
                        }
                    }
                }
            }
        // _________________________________________________________________ Scroll up ___
        else {
            // _________________________________________________ Scrollable space up ___
            if (viewPort.scrollTop < 1) {
                if (navBtn = document.getElementById(navName + "_ppbtn")) {
                    if ((now - viewPort.scrollDownReady) > 300 ||
                        viewPort.scrollHeight <= viewPort.offsetHeight) {
                        doIt                     = true;
                        viewPort.scrollDownReady = 0;
                        }
                    else {
                        viewPort.scrollDownReady = now;
                        }
                    }
                }
            }
        if (doIt) {
            stdEvent.stop();
            stdEvent.target.blur();
            o2jse.nav.btnExe(navBtn);
            }
        }

    };


/**
 * Create context menu for the db-grid
 *
 */
o2jse.tab.initContMenu = function() {

    var jxInfo = o2jse.cMenu.target.o2;
    if (jxInfo.cT == "tab") {
        // ________________________________________________ Get main table prorperties ___
        var mTb = document.getElementById(jxInfo.c + jxInfo.e);
        o2jse.ctrl.init(mTb);
        var mTbInfo = mTb.o2;
        o2jse.tab.customInfo = jxInfo;
        if (o2jse.menu.menuList["jxGridExtra"]) {
            o2jse.menu.menuList["jxGridExtra"].clear();
            }
        // ___________________ If all functions are disabled don't show grid-plus menu ___
        if (!(mTbInfo.gp_vc &&
              mTbInfo.gp_oc &&
              mTbInfo.gp_fr &&
              mTbInfo.gp_sr &&
              mTbInfo.gp_rn &&
              mTbInfo.gp_xp &&
              mTbInfo.gp_dl)) {
            o2jse.cMenu.addItem("M",
                                "jxGridExtra",
                                "Table",
                                "",
                                o2jse.rntAlias + "img/grid/grid.png");
            // _______________________________________________________ Visible columns ___
            if (!mTbInfo.gp_vc) {
                o2jse.menu.menuList["jxGridExtra"].addItem("M", "jxCustumTabCols",
                                                           "Visible columns",
                                                           "",
                                                           o2jse.rntAlias +
                                                           "img/grid/cols.png");
                o2jse.tab.visibleCols(jxInfo);
                }

            // _________________________________________________________ Order columns ___
            if (!mTbInfo.gp_oc) {
                o2jse.menu.menuList["jxGridExtra"].addItem("M", "jxGridOrder",
                                                           "Order columns",
                                                           "",
                                                           o2jse.rntAlias +
                                                           "img/grid/order.png");
                o2jse.tab.orderCols(jxInfo);
                }
            // ________________________________________________________ Filter records ___
            if (!mTbInfo.gp_fr) {
                o2jse.menu.menuList["jxGridExtra"].addItem("J", "jxGridFilter",
                                                           "Filter data",
                                                           o2jse.tab.filter,
                                                           o2jse.rntAlias +
                                                           "img/grid/filter.png");
                }
            // __________________________________________________________ Sort records ___
            if (!mTbInfo.gp_sr) {
                o2jse.menu.menuList["jxGridExtra"].addItem("M", "jxGridSort",
                                                           "Sort data",
                                                           "",
                                                           o2jse.rntAlias +
                                                           "img/grid/sort.png");
                o2jse.tab.sortCols(jxInfo);
                }
            // _______________________________________________________ Set rows number ___
            if (!mTbInfo.gp_rn) {
                o2jse.menu.menuList["jxGridExtra"].addItem("J", "jxGridRows",
                                                           "Set rows number",
                                                           o2jse.tab.setRowsN,
                                                           o2jse.rntAlias +
                                                           "img/grid/rows.png");
                }
            // ___________________________________________________________ Export data ___
            if (!mTbInfo.gp_xp) {
                o2jse.menu.menuList["jxGridExtra"].addItem("J", "jxGridExport",
                                                           "Export data",
                                                           o2jse.tab.exportData,
                                                           o2jse.rntAlias +
                                                           "img/grid/export.png");
                }
            // ___________________________________________________________ Delete data ___
            if (o2jse.tab.gridMultiDel && !mTbInfo.gp_dl) {
                if (mTbInfo.nav &&
                    document.getElementById(mTbInfo.nav + mTbInfo.e + "_delbtn")) {
                    o2jse.menu.menuList["jxGridExtra"].addItem("J", "jxGridDeleteAll",
                                                               "Delete filtered data",
                                                        function () {
                                                        o2jse.tab.deleteData(mTbInfo.nav);
                                                           },
                                                               o2jse.rntAlias +
                                                               "img/grid/delete.png");
                    }
                }
            // ________________________________ Show "save" only when settings enabled ___
            if (!(mTbInfo.gp_vc && mTbInfo.gp_oc && mTbInfo.gp_rn)) {
                // _________________________________________________________ Separator ___
                o2jse.menu.menuList["jxGridExtra"].addItem("S");
                // _____________________________________________________________ Reset ___
                o2jse.menu.menuList["jxGridExtra"].addItem("J", "jxGridReset",
                                                           "Reset",
                                                           o2jse.tab.resetCols,
                                                           o2jse.rntAlias +
                                                           "img/grid/reset.png");
                // ______________________________________________________________ Save ___
                if (o2jse.tab.autoSaveSettings == 2) {
                    o2jse.menu.menuList["jxGridExtra"].addItem("J", "jxGridSave",
                                                               "Save settings",
                                                               o2jse.tab.saveSettings,
                                                               o2jse.rntAlias +
                                                               "img/grid/save.png");
                    }
                }
            return true;
            }
        else {
            return false;
            }

        }
    else {
        return false;
        }

    };


/**
 * Executes action actName on click on columns header
 *
 * @param {object}  eventObj   Event object when fired
 * @param {integer} prgID      Program index or 0 for current program
 * @param {string}  actName    Name of action to execute
 * @param {string}  ctrlName   Name of control binding to column
 */
o2jse.tab.colExe = function(eventObj, prgID, actName, ctrlName) {

    var stdEvent  = o2jse.event.std(eventObj);
    var targetObj = stdEvent.target;
    stdEvent.stop();
    o2jse.ctrl.init(targetObj);
    // ___________________________________________________________ Create action field ___
    var colExeField = o2jse.createInput(o2jse.infoForm,
                                        "hidden",
                                        "",
                                        actName,
                                        targetObj.o2.c + "_jxcexe" + targetObj.o2.e);
    // _______________________________________________ Add external parameter as field ___
    var ctrlObj = o2jse.createInput(o2jse.infoForm,
                                    "hidden",
                                    "",
                                    ctrlName,
                                    "extp_1");

    colExeField.o2  = targetObj.o2;
    if (o2jse.cliMode) {
        jxjs.request(colExeField, "");
        }
    else {
        o2jse.cmd.ctrlUpd(colExeField);
        }
    o2jse.removeEl(colExeField);
    o2jse.removeEl(ctrlObj);
    delete colExeField;
    delete ctrlObj;
    o2jse.ctrl.make_waiting(targetObj);

    };


/**
 * Method used to create Columns Custom Visibility menu
 *
 */
o2jse.tab.visibleCols = function(jxInfo) {

    // ___________________________________ Create menu content for "Customize columns" ___
    var frameTable = document.createElement("TABLE");
    // ______________________________________________________________ Get grid control ___
    var gridObj    = document.getElementById(jxInfo.c + jxInfo.e + "_tab");
    var cols       = gridObj.getElementsByTagName("thead")[0].getElementsByTagName("th");
    var colsLen    = cols.length;
    // __________________________ Skip last pseudo-column (used to resize last column) ___
    for (var i = 0; i < colsLen - 1; i++) {
        if (cols[i].className != "o2_tab_marker") {
            o2jse.ctrl.init(cols[i]);
            var singleRow       = frameTable.insertRow(-1);
            var fieldCell       = singleRow.insertCell(-1);
            var valueCell       = singleRow.insertCell(-1);
            fieldCell.innerHTML = (cols[i].innerText || cols[i].textContent).trim();
            var checkName       = jxInfo.c + jxInfo.e + "_jxcolvis" + i;
            var checkObj        = o2jse.createInput(valueCell, "checkbox",
                                                    "o2_ctrl_check", null, checkName);
            checkObj.jxColId    = cols[i].o2.inctrl;
            if (cols[i].style.display != "none") {
                checkObj.checked = "checked";
                }
            }
        }
    singleRow           = frameTable.insertRow(-1);
    fieldCell           = singleRow.insertCell(-1);
    fieldCell.colSpan   = 4;
    fieldCell.align     = "center";
    fieldCell.innerHTML = "<input type='button' value='Apply' class='o2_ctrl_button'" +
                          "onclick='o2jse.tab.customizeCols();'>";
    o2jse.menu.menuList["jxCustumTabCols"].context(frameTable);

    };


/**
 * Method used to create Columns Custom Sorting menu
 *
 */
o2jse.tab.sortCols = function(jxInfo) {

    // ___________________________________ Create menu content for "Customize columns" ___
    var frameTable = document.createElement("TABLE");
    // ______________________________________________________________ Get grid control ___
    var gridObj    = document.getElementById(jxInfo.c + jxInfo.e + "_tab");
    var cols       = gridObj.getElementsByTagName("thead")[0].getElementsByTagName("th");
    var colsLen    = cols.length;
    var sortList   = [];
    // _______________________________________________________________ Loop on columns ___
    for (var i = 0; i < colsLen - 1; i++) {
        var singleRow = frameTable.insertRow(-1);
        var sortSpans = cols[i].getElementsByTagName("span");
        // _________________________________________________ Add only sortable columns ___
        if (sortSpans[0]) {
            o2jse.ctrl.init(cols[i]);
            var sortField      = cols[i].o2.inctrl;
            var sortTag        = " class='" + sortSpans[0].className +
                                 "' style='cursor:pointer'";
            var sortTitle      = (sortSpans[0].className == "o2_tab_sort" ?
                                  "Sort by this column ascending" :
                                  (sortSpans[0].className == "o2_tab_sorta" ?
                                   "Sort by this column descending" :
                                   "Do not sort by this column"));
            var fieldCell      = singleRow.insertCell(-1);
            var sortCell       = singleRow.insertCell(-1);
            sortCell.innerHTML = "<div onclick='o2jse.tab.sortCol(" + i + ",\"" +
                                 sortField + "\")' title='" + sortTitle + "'" + sortTag +
                                 ">&nbsp;</div>";
            var sortText       = cols[i].innerText || cols[i].textContent;
            var indexCell      = singleRow.insertCell(-1);
            var indexText      = "";
            // ____________________________________________________ Sort segment index ___
            if (sortSpans[1]) {
                indexText = (sortSpans[1].innerText || sortSpans[1].textContent);
                sortText  = sortText.trim();
                sortText  = sortText.substr(0, (sortText.length - 1));
                if (parseInt(indexText) > 0) {
                    sortList[i] = [parseInt(sortSpans[1].innerText ||
                                            sortSpans[1].textContent), sortField];
                    }
                }
            indexCell.innerHTML = "<div class='o2_tab_sorti'>" + indexText + "</div>";
            fieldCell.innerHTML = sortText.trim();
            }
        }
    o2jse.tab.sortList  = sortList;
    singleRow           = frameTable.insertRow(-1);
    fieldCell           = singleRow.insertCell(-1);
    fieldCell.colSpan   = 2;
    fieldCell.align     = "center";
    fieldCell.innerHTML = "<input type='button' value='Apply' class='o2_ctrl_button'" +
                          "onclick='o2jse.tab.customizeSort();'>";
    o2jse.menu.menuList["jxGridSort"].context(frameTable);

    };


/**
 * Method executed on click on sort image in custom sorting menu
 *
 * @param {Integer} colN
 * @param {String}  colField
 */
o2jse.tab.sortCol = function(colN, colField) {

    var frame  = o2jse.menu.menuList["jxGridSort"].inBox.getElementsByTagName("table")[0];
    var cols   = frame.getElementsByTagName("tr");
    var sDivs  = cols[colN].getElementsByTagName("div");
    var sDiv   = sDivs[0];
    var sIndex = sDivs[1];
    var sClass = sDiv.className;
    var sList  = o2jse.tab.sortList;
    var sCount = 0;
    for(k in sList) {
        if(sList[k] != undefined) {
            sCount++;
            }
        }
    // _______________________________________________________________ Unsorted column ___
    if (sClass == "o2_tab_sort") {
        sList[colN]      = [(sCount + 1), colField];
        sDiv.className   = "o2_tab_sorta";
        sIndex.innerHTML = sList[colN][0];
        }
    // _______________________________________________________ Column sorted ascending ___
    else if (sClass == "o2_tab_sorta") {
        sDiv.className = "o2_tab_sortd";
        }
    // ______________________________________________________ Column sorted descending ___
    else {
        sDiv.className = "o2_tab_sort";
        var sPos       = sList[colN][0];
        for (n = cols.length; n >= 0; n--) {
            if (sList[n] && sList[n][0] > sPos) {
                sList[n][0]--;
                cols[n].getElementsByTagName("div")[1].innerHTML = sList[n][0];
                }
            }
        sIndex.innerHTML = "";
        delete sList[colN];
        }

    };


/**
 * Method used to create Columns Custom Ordering menu
 *
 */
o2jse.tab.orderCols = function(jxInfo) {

    // ___________________________________ Create menu content for "Customize columns" ___
    var frameTable = document.createElement("TABLE");
    // ______________________________________________________________ Get grid control ___
    var gridObj    = document.getElementById(jxInfo.c + jxInfo.e + "_tab");
    var cols       = gridObj.getElementsByTagName("thead")[0].getElementsByTagName("th");
    var colsLen    = cols.length;
    var style      = 'style="cursor:pointer;margin-left:5px"';
    var start      = false;
    var singleRow;
    var fieldCell;
    var btnUpCell;
    var btnUpObj;
    var btnDownCell;
    var btnDownObj;
    // __________________________ Skip last pseudo-column (used to resize last column) ___
    for (var i = 0; i < colsLen - 1; i++) {
        if (cols[i].className != "o2_tab_marker") {
            o2jse.ctrl.init(cols[i]);
            var ctrlName        = cols[i].o2.inctrl;
            singleRow           = frameTable.insertRow(-1);
            fieldCell           = singleRow.insertCell(-1);
            fieldCell.innerHTML = (cols[i].innerText || cols[i].textContent).trim();
            btnUpCell           = singleRow.insertCell(-1);
            btnUpObj    = o2jse.createEl(btnUpCell, 'div', '',
                                         '<img src="' + o2jse.rntAlias +
                                         'img/grid/up.png" title="Move column up" ' +
                                         'onclick="o2jse.tab.orderCol(this,\'U\',\'' +
                                         ctrlName + '\');" ' + style + '>');
            // _____________________________________________ No "move up" on first row ___
            if (!start) {
                btnUpObj.firstChild.style.display = 'none';
                }
            start       = true;
            btnDownCell = singleRow.insertCell(-1);
            btnDownObj  = o2jse.createEl(btnDownCell, 'div', '',
                                         '<img src="' + o2jse.rntAlias +
                                         'img/grid/down.png" title="Move column down" ' +
                                         'onclick="o2jse.tab.orderCol(this,\'D\',\'' +
                                         ctrlName + '\');" ' + style + '>');
            }
        }
    // ____________________________________________________ No "move down" on last row ___
    btnDownObj.firstChild.style.display = 'none';
    singleRow             = frameTable.insertRow(-1);
    fieldCell             = singleRow.insertCell(-1);
    fieldCell.colSpan     = 3;
    fieldCell.align       = "center";
    fieldCell.innerHTML   = "<input type='button' value='Apply' class='o2_ctrl_button'" +
                            "onclick='o2jse.tab.customizeColsOrder();'>";
    o2jse.tab.orderList   = [];
    o2jse.menu.menuList["jxGridOrder"].context(frameTable);

    };


/**
 * Method executed on click on move up/down image in custom columns ordering menu
 *
 * @param {Object}  imgTag
 * @param {String}  mode
 */
o2jse.tab.orderCol = function(imgTag, mode, colCtrl) {

    var tr   = imgTag.parentNode.parentNode.parentNode;
    var idx  = tr.rowIndex + 1;
    // _________________________________________ Rows number (excluded "Apply" button) ___
    var rows = tr.parentNode.rows.length - 1;
    var tds;
    // _______________________________________________________________________ Move up ___
    if (mode == 'U') {
        // ______________________________________________ Second row becomes first one ___
        if (idx == 2) {
            imgTag.style.display = 'none';
            tds = tr.previousSibling.getElementsByTagName('td');
            tds[1].firstChild.firstChild.style.display = 'block';
            }
        // __________________________________________________________ Last row goes up ___
        else if (idx == rows) {
            tds = tr.getElementsByTagName('td');
            tds[2].firstChild.firstChild.style.display = 'block';
            tds = tr.previousSibling.getElementsByTagName('td');
            tds[2].firstChild.firstChild.style.display = 'none';
            }
        tr.parentNode.insertBefore(tr, tr.previousSibling);
        o2jse.tab.orderList[colCtrl] = idx - 1;
        }
    // _____________________________________________________________________ Move down ___
    else {
        // ______________________________________________ First row becomes second one ___
        if (idx == 1) {
            tds = tr.getElementsByTagName('td');
            tds[1].firstChild.firstChild.style.display = 'block';
            tds = tr.nextSibling.getElementsByTagName('td');
            tds[1].firstChild.firstChild.style.display = 'none';
            }
        // __________________________________________ Before last row becomes last one ___
        else if (idx == rows - 1) {
            imgTag.style.display = 'none';
            tds = tr.nextSibling.getElementsByTagName('td');
            tds[2].firstChild.firstChild.style.display = 'block';
            }
        tr.parentNode.insertBefore(tr.nextSibling, tr);
        o2jse.tab.orderList[colCtrl] = idx + 1;
        }

    };


/**
 * Method executed on "Apply" click from columns visibility customization
 *
 */
o2jse.tab.customizeCols = function() {

    var jxInfo = o2jse.tab.customInfo;
    var cols   = o2jse.menu.menuList["jxCustumTabCols"].inBox.
                       getElementsByTagName("input");
    var colsN  = cols.length;
    var vCols  = "";
    for (var i = 0; i < colsN; i++) {
        if (cols[i].name && !cols[i].checked) {
            vCols+= cols[i].jxColId + ";";
            }
        }
    o2jse.menu.closeAll();
    var colVisField = o2jse.createInput(o2jse.infoForm,
                                        "hidden",
                                        "",
                                        vCols.substr(0, vCols.length - 1),
                                        jxInfo.c + "_jxvcol" + jxInfo.e);
    colVisField.o2  = jxInfo;
    if (o2jse.cliMode) {
        jxjs.request(colVisField, colVisField.value);
        }
    else {
        o2jse.cmd.ctrlUpd(colVisField);
        }
    o2jse.removeEl(colVisField);

    };


/**
 * Method executed on "Apply" click from columns sort customization
 *
 */
o2jse.tab.customizeSort = function() {

    o2jse.menu.closeAll();
    var jxInfo = o2jse.tab.customInfo;
    var sList  = o2jse.tab.sortList;
    var frame  = o2jse.menu.menuList["jxGridSort"].inBox.getElementsByTagName("table")[0];
    var cols   = frame.getElementsByTagName("tr");
    // _________________________ Field value is an evaluatable PHP array coding string ___
    var resp = "array(";
    for(k in sList) {
        if(sList[k] != undefined) {
            var sDivs  = cols[k].getElementsByTagName("div");
            var sIndex = sDivs[1];
            var sClass = sDivs[0].className;
            resp      += sList[k][0] + "=>array('" +
                         (sClass == "o2_tab_sorta" ? "A" : "D") + "','" +
                         sList[k][1] + "'),";
            }
        }
    resp             = resp.substr(0, resp.length -1) + ")";
    var colSortField = o2jse.createInput(o2jse.infoForm,
                                         "hidden",
                                         "",
                                         resp,
                                         jxInfo.c + "_jxscol" + jxInfo.e);
    colSortField.o2  = jxInfo;
    if (o2jse.cliMode) {
        jxjs.request(colSortField, resp);
        }
    else {
        o2jse.cmd.ctrlUpd(colSortField);
        }
    o2jse.removeEl(colSortField);


    };


/**
 * Method executed on "Apply" click from columns ordering customization
 *
 */
o2jse.tab.customizeColsOrder = function() {

    o2jse.menu.closeAll();
    var jxInfo = o2jse.tab.customInfo;
    var oList  = o2jse.tab.orderList;
    var frames = o2jse.menu.menuList["jxGridOrder"].inBox.getElementsByTagName("table");
    var cols   = frames[0].getElementsByTagName("tr");
    // _________________________ Field value is an evaluatable PHP array coding string ___
    var resp = "array(";
    for(k in oList) {
        if(oList[k] != undefined) {
            resp+= "'" + k + "'=>" + oList[k] + ",";
            }
        }
    resp             = resp.substr(0, resp.length -1) + ')';
    var colSortField = o2jse.createInput(o2jse.infoForm,
                                         "hidden",
                                         "",
                                         resp,
                                         jxInfo.c + "_jxocol" + jxInfo.e);
    colSortField.o2  = jxInfo;
    if (o2jse.cliMode) {
        jxjs.request(colSortField, resp);
        }
    else {
        o2jse.cmd.ctrlUpd(colSortField);
        }
    o2jse.removeEl(colSortField);

    };


/**
 * Method executed on "Reset columns" item from db-grid menu
 *
 */
o2jse.tab.resetCols = function() {

    o2jse.menu.closeAll();
    var jxInfo    = o2jse.tab.customInfo;
    var sizeField = o2jse.createInput(o2jse.infoForm,
                                      "hidden",
                                      "",
                                      "1",
                                      jxInfo.c + "_jxresc" + jxInfo.e);
    sizeField.o2  = jxInfo;
    if (o2jse.cliMode) {
        jxjs.request(sizeField, sizeField.value);
        }
    else {
        o2jse.cmd.ctrlUpd(sizeField);
        }
    o2jse.removeEl(sizeField);

    };


/**
 * Method executed on "Save settings" item from db-grid menu
 *
 */
o2jse.tab.saveSettings = function() {

    o2jse.menu.closeAll();
    var jxInfo    = o2jse.tab.customInfo;
    var saveField = o2jse.createInput(o2jse.infoForm,
                                      "hidden",
                                      "",
                                      "1",
                                      jxInfo.c + "_jxsave" + jxInfo.e);
    saveField.o2  = jxInfo;
    if (o2jse.cliMode) {
        jxjs.request(saveField, saveField.value);
        }
    else {
        o2jse.cmd.ctrlUpd(saveField);
        }
    o2jse.removeEl(saveField);

    };


/**
 * Method executed on "Filter data" item from db-grid menu
 *
 */
o2jse.tab.filter = function() {

    o2jse.menu.closeAll();
    var jxInfo      = o2jse.tab.customInfo;
    var filterField = o2jse.createInput(o2jse.infoForm,
                                        "hidden",
                                        "",
                                        "1",
                                        jxInfo.c + "_jxfltr" + jxInfo.e);
    filterField.o2  = jxInfo;
    if (o2jse.cliMode) {
        jxjs.request(filterField, filterField.value);
        }
    else {
        o2jse.cmd.ctrlUpd(filterField);
        }
    o2jse.removeEl(filterField);

    };


/**
 * Method executed on "Set rows number" item from db-grid menu
 *
 */
o2jse.tab.setRowsN = function() {

    o2jse.menu.closeAll();
    var jxInfo = o2jse.tab.customInfo;
    var n      = window.prompt("Set rows number to", jxInfo.rows);
    if (parseInt(n) > 0) {
        var rowsN = o2jse.createInput(o2jse.infoForm,
                                      "hidden",
                                      "",
                                      n,
                                      jxInfo.c + "_jxrown" + jxInfo.e);
        rowsN.o2  = jxInfo;
        if (o2jse.cliMode) {
            jxjs.request(rowsN, rowsN.value);
            }
        else {
            o2jse.cmd.ctrlUpd(rowsN);
            }
        o2jse.removeEl(rowsN);
        }

    };


/**
 * Handler for click events on single dispatch notification: manage activable dispatches
 * activation
 *
 */
o2jse.tab.exportData = function() {

    o2jse.menu.closeAll();
    var jxInfo   = o2jse.tab.customInfo;
    var expField = o2jse.createInput(o2jse.infoForm,
                                     "hidden",
                                     "",
                                     "1",
                                     jxInfo.c + "_jxdexp" + jxInfo.e);
    expField.o2  = jxInfo;
    if (o2jse.cliMode) {
        jxjs.request(expField, expField.value);
        }
    else {
        o2jse.cmd.ctrlUpd(expField);
        }
    o2jse.removeEl(expField);

    };


/**
 * Method executed on "Delete data" item from db-grid menu.
 * Executes delete-action from navigator on every table row for which delete-button is
 * active.
 *
 * @param {String} navName   Navigator name to use for delete action
 */
o2jse.tab.deleteData = function(navName) {

    o2jse.menu.closeAll();
    msg = "ATTENTION! All filtered rows in grid will be deleted.\nProceed?";
    if (o2jse.cmd.confirm(msg)) {
        var jxInfo   = o2jse.tab.customInfo;
        var delField = o2jse.createInput(o2jse.infoForm,
                                         "hidden",
                                         "",
                                         navName,
                                         jxInfo.c + "_jxddel" + jxInfo.e);
        delField.o2  = jxInfo;
        if (o2jse.cliMode) {
            jxjs.request(delField, delField.value);
            }
        else {
            o2jse.cmd.ctrlUpd(delField);
            }
        o2jse.removeEl(delField);
        }

    };


/**
 * Navigator control functions collection
 *
 */
o2jse.nav = {

    btnEffectObj : null, /* Element used as buttons effect on mouse over                */
    effectTimeout : null /* Timeout to hide buttons effect on mouse over                */

    };


/**
 * Handler for OnClick events on navigator buttons
 *
 * @param object targetObj   HTML element (DIV) on which event is fired
 */
o2jse.nav.btnExe = function(targetObj) {

    o2jse.ctrl.init(targetObj);
    // __________________________________ Preserve waiting requests (submit-on-change) ___
    if (o2jse.submitting && o2jse.submitCtrl) {
        jxjs.cachedObj = targetObj;
        jxjs.cachedCmd = "if(jxjs.cachedObj.style.display != 'none' && " +
                            "jxjs.cachedObj.onclick){\n" +
                            "o2jse.submitting=false;\n" +
                            "o2jse.nav.btnExe(jxjs.cachedObj);}\n";
        return false;
        }
    if(!targetObj.o2.msg || o2jse.cmd.confirm(targetObj.o2.msg)) {
        var ctrlName                        = targetObj.o2.c + targetObj.o2.e;
        o2jse.infoForm["o2_prgexeid"].value = targetObj.o2.e;
        o2jse.infoForm[ctrlName].value      = targetObj.o2.action;
        targetObj.o2.msg                    = "";
        o2jse.ctrl.make_waiting(targetObj);
        o2jse.infoForm['o2lastform'].value = '';
        o2jse.infoForm['o2lastctrl'].value = '';
        // _____________________________________________________ Client mode execution ___
        if (o2jse.cliMode) {
            jxjs.request(targetObj, targetObj.o2.action);
            }
        else {
            o2jse.cmd.ctrlUpd(targetObj);
            }
        }

    };


/**
 * Handler for OnMouseOver events on navigator buttons
 *
 * @param object targetObj   HTML element (DIV) on which event is fired
 */
o2jse.nav.onBtn = function(targetObj) {

    if (o2jse.nav.effectTimeout) {
        clearTimeout(o2jse.nav.effectTimeout);
        }
    if (o2jse.nav.btnEffectObj) {
        o2jse.removeEl(o2jse.nav.btnEffectObj);
        o2jse.nav.btnEffectObj = null;
        }
    var bigImg             = document.createElement("div");
    var pos                = o2jse.getPos(targetObj);
    bigImg.style.position  = "absolute";
    bigImg.style.left      = pos.x + "px";
    bigImg.style.top       = pos.y + "px";
    bigImg.title           = targetObj.title;
    bigImg.onclick         = function() { bigImg.onmouseout();
                                          targetObj.onclick(targetObj); };
    bigImg.onmouseout      = function() { o2jse.removeEl(bigImg); delete bigImg; };
    bigImg.className       = targetObj.className + " ctrl_nav_effect";
    o2jse.nav.btnEffectObj = bigImg;
    o2jse.elBody.appendChild(o2jse.nav.btnEffectObj);

    };


/**
 * Data transformation functions collection
 *
 */
o2jse.data = {};


/**
 * Returns string s formatted as a string with mask mascheraStr
 *
 * @param  string s             String to format
 * @param  string mascheraStr   o2 I/O data mask
 * @return string
 */
o2jse.data.fa = function(s, mascheraStr) {

    if (s == "") {
        return "";
        }
    var r = mascheraStr.match(/\s*[ULNAC]\s*\d+/gi);
    var l = new String(s);
    var t = "";
    var q = "";
    var p = "";
    s     = "";
    ciclo:
    for (var i = 0; i < r.length; i++) {
        t = r[i].toUpperCase().substr(0,1);
        q = parseInt(r[i].substr(1));
        p = l.substr(0, q);
        l = l.substr(q);
        switch(t) {
            case "U":
                s = s + p.toUpperCase();
                break;
            case "L":
                s = s + p.toLowerCase();
                break;
            case "N":
                c = p.match(/\D+/);
                if(c) {
                    alert("Not matching picture [" + mascheraStr + "]");
                    s = "";
                    break ciclo;
                    }
                else {
                    s = s + p;
                    }
                break;
            case "A":
                c = p.match(/\W+|\d+/);
                if(c) {
                    alert("Not matching picture [" + mascheraStr + "]");
                    s = "";
                    break ciclo;
                    }
                else {
                    s = s + p;
                    }
                break;
            case "C":
                s = s + p;
                break;
            }
        }
    return s;

    };


/**
 * Returns string s formatted as a number with mask mascheraStr
 *
 * @param   {String} s             String to format
 * @param   {String} mascheraStr   o2 I/O data mask
 * @returns {String}
 */
o2jse.data.fn = function(s, mascheraStr) {

    s    = new String(s);
    r    = mascheraStr.match(/(\D*\s*)(\d+)([\.,]?)(\d*)(\D*)/);
    p    = r[1];
    i    = parseInt((r[2] || 0));
    d    = parseInt((r[4] || 0));
    l    = o2jse.decimalsPoint;
    t    = (r[5].toLowerCase().indexOf("t") > -1 ? o2jse.thousandsPoint : "");
    n    = r[5].toLowerCase().indexOf("n") > -1;
    zOff = r[5].toLowerCase().indexOf("z") > -1;
    if (s == "") {
        s = "0";
        }
    if (t != "") {
        eval("s = s.replace(/" + String.fromCharCode(92) + t + "/g, '');");
        }
    r = s.match(/([\+\-]?)\s*(\d+)([.,]?)(\d*)/);
    if (r == null) {
        r = new Array(0);
        }
    sii = "";
    if (r[1] == "-" && n) {
        sii = "-";
        }
    iib = (r[2] || '0');
    dd  = (r[4] || '0');
    ff  = parseFloat(iib + "." + dd).toFixed(d);
    iib = String(parseInt(ff)).substr(0 - i);
    dd  = String(ff).split(".")[1] || 0;
    iib = iib.substr(0 - i);
    ii  = "";
    for (var iiind = 0; iiind < iib.length; iiind++) {
        iichar = iib.charAt(iib.length - iiind - 1);
        ii = iichar + ii;
        if ((((iiind + 1) % 3) == 0) && (iiind < (iib.length - 1))) {
            ii = t + ii;
            }
        }
    s  = p + sii + ii + (dd ? l + dd : "");
    if (parseFloat(ii) == 0 && parseFloat(dd) == 0 && zOff) {
        s = "";
        }
    return s;

    };


/**
 * Returns string s formatted as a date with mask mascheraStr
 *
 * @param  string s             String to format
 * @param  string mascheraStr   o2 I/O data mask
 * @return string
 */
o2jse.data.fd = function(s, mascheraStr) {

    if (mascheraStr.indexOf('2') > -1) {
        mascheraStr = mascheraStr.replace(/2/g, '');
        sy          = true;
        }
    else {
        sy = false;
        }
    s        = new String(s);
    var zOff = (mascheraStr.toLowerCase().indexOf('z') > -1);
    var l    = (mascheraStr.charAt(0) || "/");
    var e1   = (mascheraStr.charAt(1).toLowerCase() || "");
    var e2   = (mascheraStr.charAt(2).toLowerCase() || "");
    e2       = (e2 == "2" || e2 == "z" ? "" : e2);
    var e3   = (mascheraStr.charAt(3).toLowerCase() || "");
    e3       = (e3 == "2" || e3 == "z" ? "" : e3);
    if ((e1 == "") && (e2 == "") && (e3 == "")) {
        e1 = "d";
        e2 = "m";
        e3 = "y";
        }
    e = (e1 != "" ? e1 + (e2 != "" || e3 != "" ? l : "") : "") +
        (e2 != "" ? e2 + (e3 != "" ? l : "") : "") +
        e3;
    if (s.replace(/\D/g, "") == "") {
        s = (zOff ? "" : e.replace(/[dm]/g, "00").replace(/y/g, (sy ? "00" : "0000")));
        }
    else {
        s  = s.replace(/\D/g, '');
        tc = 2;
        cd = -1;
        cm = -1;
        cy = -1;
        switch(e1) {
            case "d":
                cd = 0;
                break;
            case "m":
                cm = 0;
                break;
            case "y":
                cy = 0;
                tc = (sy ? 2 : 4);
                break;
            }
        switch(e2) {
            case "d":
                cd = tc;
                tc = tc + 2;
                break;
            case "m":
                cm = tc;
                tc = tc + 2;
                break;
            case "y":
                cy = tc;
                tc = tc + (sy ? 2 : 4);
                break;
            }
        switch(e3) {
            case "d":
                cd = tc;
                tc = tc + 2;
                break;
            case "m":
                cm = tc;
                tc = tc + 2;
                break;
            case "y":
                cy = tc;
                tc = tc + (sy ? 2 : 4);
                break;
            }
        x = new Date();
        d = (s.charAt(cd) || "0") + (s.charAt(cd + 1) || "0");
        if (parseInt(d, 10) == 0) {
            d = "01";
            }
        m = (s.charAt(cm) || "0") + (s.charAt(cm + 1) || "0");
        if (parseInt(m, 10) == 0) {
            m = "01";
            }
        y = (s.charAt(cy) ?
                      (sy || !s.charAt(cy + 3) ?
                      '20' + (s.charAt(cy) || "0") + (s.charAt(cy + 1) || "0") :
                      (s.charAt(cy) || "0") +
                      (s.charAt(cy + 1) || "0") +
                      (s.charAt(cy + 2) || "0") +
                      (s.charAt(cy + 3) || "0")) :
                     String(x.getFullYear()));
        o = new Date(y, (m - 1), d);
        if (o.getDate() != d || o.getMonth() != (m - 1) || o.getFullYear() != y) {
            msk = e.replace(/d/g, "DD").
                    replace(/m/g, "MM").
                    replace(/y/g, (sy ? "YY" : "YYYY"));
            alert('Not a valid date.\nMatch input mask "' + msk + '"');
            s = (zOff ?
                 "" :
                 e.replace(/[dm]/g, "00").replace(/y/g, (sy ? "00" : "0000")));
            }
        else {
            s = e.replace(/d/g, d).
                    replace(/m/g, m).
                    replace(/y/g, (sy ? y.charAt(2) + y.charAt(3) : y));
            }
        }
    return s;

    };


/**
 * Returns string s formatted as a time with mask mascheraStr
 *
 * @param  string s             String to format
 * @param  string mascheraStr   o2 I/O data mask
 * @return string
 */
o2jse.data.ft = function(s, mascheraStr) {

    s    = new String(s);
    zOff = (mascheraStr.toLowerCase().indexOf('z') > -1);
    l    = (mascheraStr.charAt(0)||':');
    e1   = (mascheraStr.charAt(1).toLowerCase() || "");
    e2   = (mascheraStr.charAt(2).toLowerCase() || "");
    e2   = (e2 == "z" ? "" : e2);
    e3   = (mascheraStr.charAt(3).toLowerCase() || "");
    e3   = (e3 == "z" ? "" : e3);
    if ((e1 == "") && (e2 == "") && (e3 == "")) {
        e1 = "h";
        e2 = "m";
        e3 = "s";
        }
    e = (e1 != "" ? e1 + (e2 != "" || e3 != "" ? l : "") : "") +
        (e2 != "" ? e2 + (e3 != "" ? l : "") : "") +
        e3;
    if (s.replace(/\D/g, "") == "") {
        s = (zOff ? "" : e.replace(/[hms]/g, "00"));
        }
    else {
        s  = s.replace(/\D/g, "");
        tc = 2;
        ch = -1;
        cm = -1;
        cs = -1;
        ph = false;
        pm = false;
        ps = false;
        switch(e1) {
            case "h":
                ch = 0;
                ph = true;
                break;
            case "m":
                cm = 0;
                pm = true;
                break;
            case "s":
                cs = 0;
                ps = true;
                break;
            }
        switch(e2) {
            case "h":
                ch = tc;
                tc = tc + 2;
                ph = true;
                break;
            case "m":
                cm = tc;
                tc = tc + 2;
                pm = true;
                break;
            case "s":
                cs = tc;
                tc = tc + 2;
                ps = true;
                break;
            }
        switch(e3) {
            case "h":
                ch = tc;
                tc = tc + 2;
                ph = true;
                break;
            case "m":
                cm = tc;
                tc = tc + 2;
                pm = true;
                break;
            case "s":
                cs = tc;
                tc = tc + 2;
                ps = true;
                break;
            }

        h = (ph ? (s.charAt(ch) || "0") + (s.charAt(ch + 1) || "") : "00");
        m = (pm ? (s.charAt(cm) || "0") + (s.charAt(cm + 1) || "") : "00");
        s = (ps ? (s.charAt(cs) || "0") + (s.charAt(cs + 1) || "") : "00");
        h = (h.length == 2 ? '' : "0") + h;
        m = (m.length == 2 ? '' : "0") + m;
        s = (s.length == 2 ? '' : "0") + s;

        o = new Date(0,0,0,h,m,s);
        if (o.getHours() != h || o.getMinutes() != m || o.getSeconds() != s) {
            m = e.replace(/h/g, "HH").replace(/m/g, "MM").replace(/s/g, "SS");
            alert('Not a valid time.\nMatch input mask "' + m + '"');
            s = (zOff ? "" : e.replace(/[hms]/g, "00"));
            }
        else {
            s = e.replace(/h/g, h).replace(/m/g, m).replace(/s/g, s);
            }
        }
    return s;

    };


/**
 * Javascript executable commands collection
 *
 * @type {object}
 */
o2jse.cmd = {};


/**
 * Sets o2 control targetObj as changed.
 * Control object has to be passed by reference.
 *
 * @param {object}  targetObj   o2 control object to be set as changed
 */
o2jse.cmd.ctrlUpd = function(targetObj) {

    o2jse.ctrl.init(targetObj);
    infoObj = targetObj.o2;
    if (String(infoObj.msg) == "" || o2jse.cmd.confirm(infoObj.msg)) {
        modFields = o2jse.infoForm['o2_modfields'];
        if (modFields.value.indexOf(infoObj.c + ";") < 0) {
            modFields.value += infoObj.c + ";";
            }
        if (infoObj.fret) {
            o2jse.cmd.submit(infoObj.e);
            }
        }

    };


/**
 * Submits o2 form
 *
 */
o2jse.cmd.submit = function(exeId) {

    if (o2jse.submitting) {
        if (!o2jse.cliMode) {
            alert("Requested action not executed: data validation was needed before.\n" +
                  "Please try again.");
            return true;
            }
        else {
            return o2jse.stopExe();
            }
        }
    // __________________________________________________________________ Client width ___
    var lcsW = window.innerWidth;
    if (parseInt(lcsW) > 0) {
        o2jse.infoForm['jxcsw'].value = lcsW;
        }
    // _________________________________________________________________ Client height ___
    var lcsH = window.innerHeight;
    if (parseInt(lcsH) > 0) {
        o2jse.infoForm['jxcsh'].value = lcsH;
        }
    // ______________________________________________________________________ Menu bar ___
    if (o2jse.menu.appMainMenu) {
        // _____________________________________________ Menu bar TOP - vertical space ___
        if (o2jse.menuStyle == 'T') {
            var lmbH = document.getElementById("jxMenuBar").offsetHeight;
            if (parseInt(lmbH) > 0) {
                o2jse.infoForm['jxmbh'].value = lmbH;
                }
            }
        // __________________________________________ Menu bar LEFT - horizontal space ___
        else {
            var lmbW = document.getElementById("jxMenuBar").offsetWidth;
            if (parseInt(lmbW) > 0) {
                o2jse.infoForm['jxmbw'].value = lmbW;
                }
            }
        }
    // ___________________________________________________ Status Bar - vertical space ___
    if (statusBar = document.getElementById("o2status")) {
        o2jse.infoForm['jxsbh'].value = statusBar.offsetHeight;
        }
    o2jse.infoForm['o2_prgexeid'].value = exeId;
    o2jse.submitting                    = true;
    o2jse.infoForm.submit();
    o2jse.infoForm['o2lastctrl'].value = "";
    o2jse.infoForm['o2lastform'].value = "";

    };


/**
 * Executes a Go-to-URL with parameters using POST.
 *
 * Parameters are a list of properties or associative array with names and values:
 *
 * {par1:'val1', par2:'val2', ...}
 *
 * If param newWindow is passed as TRUE then targetURL is opened in a new browser window.
 *
 * This method is intended to be used to replace window relocation that uses GET.
 *
 * @param {String}  targetURL
 * @param {Object}  params
 * @param {Boolean} newWindow
 */
o2jse.cmd.post = function(targetURL, params, newWindow) {

    // ________________________________________________________ Compose form to submit ___
    var form = document.createElement('form');
    form.method  = "POST";
    form.enctype = o2jse.infoForm.enctype;
    if (targetURL) {
        form.action = targetURL;
        }
    else {
        form.action = o2jse.infoForm.action;
        }
    // _________________________________________________ Add parameters as form fields ___
    for (n in params) {
        f = document.createElement("input");
        f.setAttribute("type", "hidden");
        f.setAttribute("name", n);
        f.setAttribute("value", params[n]);
        form.appendChild(f);
        f = null;
        }
    if (newWindow) {
        win         = 'V' + (+new Date).toString(36).slice(-5);
        form.target = win;
        window.open('', win);
        }
    document.body.appendChild(form);
    o2jse.submitting = true;
    form.submit();
    document.body.removeChild(form);
    form = null;

    };


/**
 * Return back to server to run program prgName and pass other parameters
 *
 * @param {String} prgName   Name of program to run
 */
o2jse.cmd.run = function(prgName) {

    // ________________________________________________________ Fields to be submitted ___
    fields = [];
    // ________________________________________________________________ Sesssion stuff ___
    fields['o2c']        = prgName;
    fields['JXSESSNAME'] = o2jse.sessName;
    if (o2jse.infoForm[o2jse.sessName]) {
        fields[o2jse.sessName] = o2jse.infoForm[o2jse.sessName].value;
        }
    // _________________________________________________________ Add passed parameters ___
    if (arguments.length > 1) {
        for (var argID = 1; argID < arguments.length; argID++) {
            fields["extp_" + argID] = arguments[argID];
            }
        }
    // _____________________________________________________________________ Menu size ___
    if (menuBar = document.getElementById("jxMenuBar")) {
        if (o2jse.menuStyle == 'T') {
            fields['jxmbh'] = menuBar.offsetHeight;
            }
        else {
            fields['jxmbw'] = menuBar.offsetWidth;
            }
        }
    // ____________________________________________________________________ Status Bar ___
    if (statusBar = document.getElementById("o2status")) {
        fields['jxsbh'] = statusBar.offsetHeight;
        }
    // ____________________________________________________________ Client window size ___
    fields['jxcsw'] = (window.innerWidth != null ?
                       window.innerWidth : document.documentElement.clientWidth);
    fields['jxcsh'] = (window.innerHeight != null ?
                       window.innerHeight : document.documentElement.clientHeight);
    // _________________________________ Compose call path - descending menu hierarchy ___
    var pathStr = '';
    for (var i = 0; i < o2jse.menu.openMenus.length; i++) {
        if (typeof o2jse.menu.openMenus[i].label == 'undefined') {
            pathStr = "jxContext";
            }
        else {
            pathStr+= (pathStr ? '|' : '') + o2jse.menu.openMenus[i].label;
            }
        }
    fields['o2p'] = pathStr;
    // _______________________________________________________________ Post parameters ___
    o2jse.cmd.post(false, fields);
    };


/**
 * Executes action actName in Janox program with execution_id = prgID
 *
 * @param {object}  eventObj   Event object when fired
 * @param {integer} prgID      Program index or 0 for current program
 * @param {string}  actName    Name of action to execute
 */
o2jse.cmd.exe = function(eventObj, prgID, actName) {

    if (prgID > 0) {
        o2jse.infoForm['o2_prgexeid'].value = prgID;
        }
    o2jse.infoForm['o2_action'].value   = actName;
    o2jse.infoForm['o2lastform'].value  = "";
    o2jse.infoForm['o2lastctrl'].value  = "";
    // ____________________ Remove previous external parameters fields (Full-AJAX only)___
    for (var i = o2jse.infoForm.elements.length; i > 0; i--) {
        var field = o2jse.infoForm.elements[i - 1];
        if (field.type == "hidden" && field.name.substr(0, 5) == "extp_") {
            o2jse.removeEl(o2jse.infoForm.elements[i - 1]);
            }
        }
    // ______________________________________________ Add external parametrs as fields ___
    if (arguments.length > 3) {
        o2jse.extPars = [];
        for (var argID = 3; argID < arguments.length; argID++) {
            o2jse.extPars[argID - 3] = o2jse.createInput(o2jse.infoForm,
                                                         "hidden",
                                                         "",
                                                         arguments[argID],
                                                         "extp_" + (argID - 2));
            }
        }
    // __________________________ If NOT fired from inside table row (not current row) ___
    var stdEvent = o2jse.event.std(eventObj);
    if (!(stdEvent.target && stdEvent.target.o2.tabRow)) {
        stdEvent.stop();
        if (o2jse.cliMode) {
            jxjs.request();
            }
        else {
            o2jse.cmd.submit(prgID);
            }
        }
    if (stdEvent.target && stdEvent.target.tagName) {
        o2jse.ctrl.make_waiting(stdEvent.target);
        }

    };


/**
 * Sets focus to the requested control. If Janox JavaScript environment is not yet
 * started, field to focus is recorded and then focused later.
 *
 * @param string ctrlName   Name of control to set focus to
 */
o2jse.cmd.focus = function(ctrlName, seleMode) {

    if (o2jse.started) {
        o2jse.ctrl.focusCtrl                 = null;
        o2jse.ctrl.focusMode                 = null;
        o2jse.infoForm['o2lastctrl'].value   = "";
        o2jse.infoForm['o2lastform'].value   = "";
        // __________________________________________ Get INPUT fields and button-DIVs ___
        var ctrlLocal = o2jse.infoForm[ctrlName] || document.getElementById(ctrlName);
        if (ctrlLocal && ctrlLocal.focus) {
            o2jse.ctrl.init(ctrlLocal);
            // _________________________________________________________________ Table ___
            if (ctrlLocal.o2.cT == "tab") {
                var markName = ctrlLocal.o2.c + "_sele" + ctrlLocal.o2.e;
                if (o2jse.infoForm[markName]) {
                    o2jse.infoForm[markName].focus();
                    }
                }
            // __________________________________________________________ Combo lookup ___
            else if (ctrlLocal.o2.cT == "listcombo" &&
                     o2jse.infoForm[ctrlName + "_desc"]) {
                ctrlLocal = o2jse.infoForm[ctrlName + "_desc"];
                ctrlLocal.focus();
                if (ctrlLocal.select) {
                    ctrlLocal.select();
                    }
                }
            // ______________________________________________________________ Treeview ___
            else if (ctrlLocal.o2.cT == "tree") {
                if (ctrlLocal = document.getElementById(ctrlLocal.o2.c + '_jxFocusNode')){
                    ctrlLocal.focus();
                    }
                }
            // ____________________________________________________ All other controls ___
            else {
                ctrlLocal.focus();
                // ______________________________________________________ Select value ___
                if (seleMode == null) {
                    if (ctrlLocal.select) {
                        ctrlLocal.select();
                        }
                    }
                // _____________________________________________________ Move to start ___
                else if (seleMode == 0) {
                    if (ctrlLocal.setSelectionRange) {
                        ctrlLocal.setSelectionRange(0, 0);
                        }
                    ctrlLocal.scrollTop = 0;
                    }
                // _______________________________________________________ Move to end ___
                else {
                    if (ctrlLocal.setSelectionRange) {
                        var len = ctrlLocal.value.length * 2;
                        ctrlLocal.setSelectionRange(len, len);
                        }
                    else {
                        ctrlLocal.value = ctrlLocal.value;
                        }
                    ctrlLocal.scrollTop = 999999;
                    }
                }
            }
        }
    else {
        o2jse.ctrl.focusCtrl = ctrlName;
        o2jse.ctrl.focusMode = seleMode;
        }

    };


/**
 * Find next control able to get focus, starting from startCtrl.
 *
 * @param  {Object} startCtrl   control element to start from to find next one
 * @return {Object}
 */
o2jse.cmd.getNextFocusCtrl = function(startCtrl) {

    var startCtrlId = startCtrl.id;
    var formElems   = o2jse.infoForm.elements;
    for (i = (formElems.length - 1); i >= 0; i--) {
        if (formElems[i].id == startCtrlId) {
            var nextId = i + 1;
            while (!(formElems[nextId].id || formElems[nextId].name) ||
                   !formElems[nextId].focus ||
                   formElems[nextId].type == "hidden" ||
                   formElems[nextId].style.display == "none") {
                nextId++;
                }
            if (!formElems[nextId]) {
                nextId = 0;
                }
            break;
            }
        }
    return formElems[nextId];

    };


/**
 * Show a confirm window with a message and "OK" and "CANCEL" buttons: returns TRUE if
 * "OK" button is pressed, FALSE otherwise.
 *
 * @param string confMsg   Message string to show
 */
o2jse.cmd.confirm = function(confMsg) {

    return confirm(confMsg);

    };


/**
 * Create a full screen "glass" for events catching while windows moving and sizing
 *
 * @param {Function} funcMouseMove   Function to be registered for onMouseMove events
 * @param {Function} funcMouseUp     Function to be registered for onMouseUp events
 * @param {String}   mouseStyle      Cursor style while "glassing"
 * @param {Boolean}  darkOut         Show dark (not transparent) glass
 * @param {Boolean}  escable         If action can be stopped pressing ESC
 */
o2jse.cmd.startGlass = function(funcMouseMove,
                                funcMouseUp,
                                mouseStyle,
                                darkOut,
                                escable) {

    if (typeof mouseStyle != "string" || mouseStyle == "") {
        mouseStyle = "default";
        }
    o2jse.glassObj                = o2jse.createEl(o2jse.elBody, "DIV");
    o2jse.glassObj.style.position = "absolute";
    o2jse.glassObj.style.top      = "0";
    o2jse.glassObj.style.left     = "0";
    o2jse.glassObj.style.zIndex   = 999999;
    o2jse.glassObj.style.cursor   = mouseStyle;
    document.onmousemove          = funcMouseMove;
    document.onmouseup            = funcMouseUp;
    document.onselectstart        = function() { return false; };
    o2jse.glassObj.oncontextmenu  = function(e) { o2jse.event.std(e).stop();
                                                  return false; };
    // ____________________________________________________________________ Glass size ___
    o2jse.glassObj.style.width  = "100%";
    o2jse.glassObj.style.height = "100%";
    // ____________________________________________________________________ Dark glass ___
    if (darkOut) {
        o2jse.glassObj.style.backgroundColor = "#000000";
        o2jse.cmd.opacity(o2jse.glassObj, 33);
        }
    else {
        o2jse.glassObj.style.backgroundColor = "transparent";
        }
    if (escable) {
        document.onkeydown = function(e) { document.onkeydown = null;
                                          funcMouseUp(e, true); };
        }

    };


/**
 * Stops full screen "glass"
 *
 */
o2jse.cmd.stopGlass = function() {

    if (o2jse.glassObj != null) {
        o2jse.removeEl(o2jse.glassObj);
        document.onmousemove   = null;
        document.onmouseup     = null;
        document.onselectstart = null;
        o2jse.glassObj         = null;
        }

    };


/**
 * Requests help from JXDocumentor for the passed element
 *
 * @param {Object} targetObj    Control or form on which documentation is requested
 */
o2jse.cmd.doc = function(targetObj) {

    o2jse.ctrl.init(targetObj);
    var infoObj = targetObj.o2;
    o2jse.createInput(o2jse.infoForm, "hidden", "", "help", "jxdocreq");
    o2jse.createInput(o2jse.infoForm, "hidden", "", infoObj.f, "o2_formname");
    o2jse.createInput(o2jse.infoForm, "hidden", "", infoObj.c, "o2_ctrlname");
    o2jse.cmd.submit(0);

    };


/**
 * Shows upload waiting image
 *
 * @param {Boolean} darkOut   If a dark glass must cover the page while waiting
 */
o2jse.cmd.showClock = function(darkOut) {

    if (!o2jse.clockObj) {
        o2jse.clockObj = o2jse.createEl(document.body, "DIV", "jxclock", "&nbsp;");
        o2jse.cmd.startGlass(null, null, "wait", darkOut);
        }

    };


/**
 * Hides upload waiting image
 *
 */
o2jse.cmd.hideClock = function() {

    if (o2jse.clockObj) {
        o2jse.removeEl(o2jse.clockObj);
        o2jse.clockObj = null;
        o2jse.cmd.stopGlass();
        }

    };


/**
 * Sets opacity for HTML object fadingObj in different browsers
 *
 * @param string  fadingObj   ID of object to faded out
 * @param integer opacity     Actual object opacity
 */
o2jse.cmd.opacity = function(targetObj, opacity) {

    if (targetObj && targetObj.style) {
        targetObj.style.opacity = opacity / 100;
        }

    };


/**
 * Play a sound file
 *
 * @param string  soundFile   Name of the audio file to play
 */
o2jse.cmd.playSound = function(soundFile, mimeType) {

    var sPlayer;
    sPlayer = document.createElement("audio");
    // _____________________________________________________ If AUDIO tag is supported ___
    if (sPlayer != null && sPlayer.canPlayType && sPlayer.canPlayType(mimeType)) {
        sPlayer.setAttribute("src", soundFile);
        sPlayer.play();
        }
    // _____________________________________ Embed audio file to be played by a plugin ___
    else {
        // _______________________________________ Destroy player if it already exists ___
        if (o2jse.soundPlayer) {
            o2jse.removeEl(o2jse.soundPlayer);
            }
        // _________________________________________________________ Create EMBED node ___
        o2jse.soundPlayer = document.createElement("embed");
        if (o2jse.soundPlayer != null) {
            o2jse.soundPlayer.setAttribute("src", soundFile);
            o2jse.soundPlayer.setAttribute("type", mimeType);
            o2jse.soundPlayer.setAttribute("with", "0");
            o2jse.soundPlayer.setAttribute("height", "0");
            o2jse.soundPlayer.setAttribute("autostart", "true");
            o2jse.soundPlayer.setAttribute("loop", "false");
            // ___________________________________________________________ Add to body ___
            (o2jse.elBody  || document.body || document.getElementsByTagName("body")[0]).
             appendChild(o2jse.soundPlayer);
            }
        }

    };


/**
 * Windows related functions collection
 *
 */
o2jse.win = {

    movingWin   : [],     /* List of references to main windows elements (DIV)          */
    dPos        : [],     /* List of delta positions while moving / sizing              */
    linkedWins  : 0,      /* Number of linked windows to keep together                  */
    winX        : 0,      /* Mouse pointer X while moving/sizing                        */
    winY        : 0,      /* Mouse pointer Y while moving/sizing                        */
    showMenu    : false   /* Window menu (click on doc) is shown                        */

    };


/**
 * Starts action of window moving
 *
 * @param object targetObj   Object on which event is fired
 * @param object eventObj    On click event object
 */
o2jse.win.startMoving = function(targetObj, eventObj) {

    o2jse.ctrl.init(targetObj);
    var stdEvent = o2jse.event.std(eventObj);
    if (stdEvent.button != 2) {
        stdEvent.stop();
        for(var index = targetObj.o2.subf; index >= 0; index--) {
            var suff    = (index > 0 ? String(index) : "");
            var winName = targetObj.o2.f + "_" + targetObj.o2.e + suff;
            if (o2jse.win.movingWin[index] = document.getElementById(winName)) {
                o2jse.win.dPos[index] = {x:(o2jse.win.movingWin[index].offsetLeft -
                                            stdEvent.x),
                                         y:(o2jse.win.movingWin[index].offsetTop -
                                            stdEvent.y)};
                o2jse.cmd.opacity(o2jse.win.movingWin[index], 80);
                }
            else {
                delete o2jse.win.movingWin[index];
                }
            }
        o2jse.cmd.startGlass(o2jse.win.moveTo, o2jse.win.stopMoving, "move");
        }

    };


/**
 * Executes action of window moving
 *
 * @param object eventObj   Mouse move event object
 */
o2jse.win.moveTo = function(eventObj) {

    var menuBarH = 0;
    var stdEvent = o2jse.event.std(eventObj);
    stdEvent.stop();
    if (o2jse.menu.appMainMenu && o2jse.menuStyle == 'T') {
        menuBarH = document.getElementById("jxMenuBar").offsetHeight;
        }
    for(var index in o2jse.win.movingWin) {
        var lX = (stdEvent.x + o2jse.win.dPos[index].x);
        var lY = Math.max(stdEvent.y + o2jse.win.dPos[index].y, menuBarH);
        if (o2jse.win.movingWin[index] != null) {
            o2jse.win.movingWin[index].style.left = lX + 'px';
            o2jse.win.movingWin[index].style.top  = lY + 'px';
            }
        }

    };


/**
 * Stops action of window moving
 *
 * @param object eventObj   Mouse up event object
 */
o2jse.win.stopMoving = function(eventObj) {

    var stdEvent = o2jse.event.std(eventObj);
    stdEvent.stop();
    o2jse.cmd.stopGlass();
    for (var index in o2jse.win.movingWin) {
        o2jse.cmd.opacity(o2jse.win.movingWin[index], 100);
        o2jse.win.needNoRepos(o2jse.win.movingWin[index].id);
        }
    o2jse.win.savePos(o2jse.firstRealChild(o2jse.win.movingWin[0]));
    o2jse.win.movingWin = [];
    o2jse.win.dPos      = [];

    };


/**
 * Starts action of window sizing
 *
 * @param object targetObj   Object on which event is fired
 * @param object eventObj    On click event object
 */
o2jse.win.startSizing = function(targetObj, eventObj) {

    o2jse.ctrl.init(targetObj);
    var stdEvent = o2jse.event.std(eventObj);
    if (stdEvent.button != 2) {
        stdEvent.stop();
        var winName            = targetObj.o2.f + "_" + targetObj.o2.e;
        var winShadow          = document.getElementById(winName);
        winShadow.className    = "";
        o2jse.win.movingWin[0] = o2jse.firstRealChild(winShadow);
        o2jse.win.movingWin[1] = document.getElementById(winName + "_in");
        o2jse.win.dPos[0]      = {x:(o2jse.win.movingWin[0].offsetWidth  - stdEvent.x),
                                  y:(o2jse.win.movingWin[1].offsetHeight - stdEvent.y)};
        o2jse.cmd.opacity(o2jse.win.movingWin[0], 80);
        o2jse.cmd.startGlass(o2jse.win.sizeTo, o2jse.win.stopSizing, "se-resize");
        }

    };


/**
 * Executes action of window sizing
 *
 * @param object eventObj   Mouse move event object
 */
o2jse.win.sizeTo = function(eventObj) {

    var stdEvent = o2jse.event.std(eventObj);
    var lX       = (stdEvent.x + o2jse.win.dPos[0].x);
    var lY       = (stdEvent.y + o2jse.win.dPos[0].y);
    stdEvent.stop();
    o2jse.win.movingWin[0].style.width  = lX + 'px';
    o2jse.win.movingWin[1].style.height = lY + 'px';

    };


/**
 * Stops action of window sizing
 *
 * @param object eventObj   Mouse up event object
 */
o2jse.win.stopSizing = function(eventObj) {

    var stdEvent = o2jse.event.std(eventObj);
    stdEvent.stop();
    o2jse.ctrl.init(o2jse.win.movingWin[0]);
    formInfo = o2jse.win.movingWin[0].o2;
    // ________________________________________________________________ Refresh action ___
    var refrAct = formInfo.r;
    o2jse.cmd.stopGlass();
    o2jse.cmd.opacity(o2jse.win.movingWin[0], 100);
    o2jse.win.saveSize(o2jse.win.movingWin[0]);
    o2jse.win.movingWin[0].parentNode.className = "winshadow";
    o2jse.win.movingWin                         = [];
    o2jse.win.dPos                              = [];
    // _______ Set <form>_jxcmd = '3' to remove 'maximized' flag on form (server-side) ___
    var cmdField   = o2jse.infoForm[formInfo.f + formInfo.e + "_jxcmd"];
    cmdField.value = '3';
    o2jse.infoForm['o2_modfields'].value+= cmdField.name + ";";
    // ______________________________________________________ Refresh action on resize ___
    jxjs.refresh(formInfo, refrAct);

    };


/**
 * Save windows position
 *
 * @param object winObj   Window external DIV element
 */
o2jse.win.savePos = function(winObj) {

    var lmbH = 0;
    var lmbW = 0;
    if (o2jse.menu.appMainMenu &&
        document.getElementById("jxMenuBar")) {
        if (o2jse.menuStyle != 'T') {
            var lmbW = parseInt(document.getElementById("jxMenuBar").offsetWidth);
            }
        else {
            var lmbH = parseInt(document.getElementById("jxMenuBar").offsetHeight);
            }
        }
    var objId     = winObj.parentNode.id;
    var inWin     = document.getElementById(objId + "_in");
    var newStr    = parseInt(winObj.parentNode.offsetLeft + winObj.offsetLeft - lmbW) +
                    "," +
                    parseInt(winObj.parentNode.offsetTop + winObj.offsetTop - lmbH) + ";";
    var winList   = o2jse.infoForm['win_list'].value.split(";");
    var posList   = o2jse.infoForm['win_pos'].value.split(";");
    var sizeList  = o2jse.infoForm['win_size'].value.split(";");
    var posStr    = "";
    var sizeStr   = "";
    var toAdd     = true;
    for (var winIndex in winList) {
        if (winList[winIndex] != "") {
            if (winList[winIndex] != objId) {
                posStr += posList[winIndex] + ";";
                sizeStr+= sizeList[winIndex] + ";";
                }
            else {
                posStr += newStr;
                sizeStr+= sizeList[winIndex] + ";";
                toAdd  = false;
                }

            }
        }
    if (toAdd) {
        o2jse.infoForm['win_list'].value+= objId + ";";
        posStr                          += newStr;
        sizeStr                         += ",;";
        }
    o2jse.infoForm['win_pos'].value  = posStr;
    o2jse.infoForm['win_size'].value = sizeStr;

    };


/**
 * Save windows size
 *
 * @param object winObj   Window external DIV element
 */
o2jse.win.saveSize = function(winObj) {

    var objId    = winObj.parentNode.id;
    var inWin    = document.getElementById(objId + "_in");
    var newStr   = parseInt(winObj.style.width) + "," +
                   (parseInt(inWin.offsetHeight) + 30) + ";";
    var winList  = o2jse.infoForm['win_list'].value.split(";");
    var posList  = o2jse.infoForm['win_pos'].value.split(";");
    var sizeList = o2jse.infoForm['win_size'].value.split(";");
    var posStr   = "";
    var sizeStr  = "";
    var toAdd    = true;
    for (var winIndex in winList) {
        if (winList[winIndex] != "") {
            if (winList[winIndex] != objId) {
                posStr += posList[winIndex] + ";";
                sizeStr+= sizeList[winIndex] + ";";
                }
            else {
                posStr += posList[winIndex] + ";";
                sizeStr+= newStr;
                toAdd  = false;
                }

            }
        }
    if (toAdd) {
        o2jse.infoForm['win_list'].value+= objId + ";";
        posStr                          += ",;";
        sizeStr                         += newStr;
        }
    o2jse.infoForm['win_pos'].value  = posStr;
    o2jse.infoForm['win_size'].value = sizeStr;

    };


/**
 * Maximize/restore o2 window
 *
 * @param object targetObj   Object on which event is fired
 */
o2jse.win.maxRest = function(targetObj) {

    o2jse.ctrl.init(targetObj);
    var objInfo  = targetObj.o2;
    var winDiv   = document.getElementById(objInfo.f + "_" + objInfo.e);
    var cmdField = o2jse.infoForm[objInfo.f + objInfo.e + "_jxcmd"];
    o2jse.ctrl.init(winDiv);
    if (winDiv.o2.max) {
        o2jse.maximizedWin = false;
        }
    // ______________________________ Conditions for max-rest are the same as for exit ___
    if (winDiv.o2.exit) {
        cmdField.value = "2";
        o2jse.infoForm['o2_modfields'].value+= cmdField.name + ";";
        o2jse.infoForm['o2lastform'].value   = objInfo.f;
        o2jse.infoForm['o2lastctrl'].value   = "";
        // __________________________________________________ Refresh action on resize ___
        var refrAct = winDiv.o2.r || '';
        jxjs.refresh(winDiv.o2, refrAct);
        }

    };


/**
 * Minimize/restore o2 window
 *
 * @param string o2w_form
 * @param string open_class
 * @param string close_class
 * @return boolean
 */
o2jse.win.openClose = function(o2w_form, open_class, close_class) {

    var o2_form_in  = document.getElementById(o2w_form + "_in");
    var o2_form_btn = document.getElementById(o2w_form + "_btn");
    if (o2_form_in.style.display == 'none') {
        o2_form_in.style.display = 'block';
        o2_form_btn.className    = close_class;
        }
    else {
        o2_form_in.style.display = 'none';
        o2_form_btn.className    = open_class;
        }
    return false;

    };


/**
 * Event handler for window menu events
 *
 */
o2jse.win.menu = function(targetObj) {

    if (o2jse.win.showMenu) {
        o2jse.menu.closeAll();
        o2jse.win.showMenu = false;
        return false;
        }
    o2jse.ctrl.init(targetObj);
    var posObj = {x: targetObj.offsetLeft,
                  y: targetObj.offsetTop + targetObj.offsetHeight};
    var cont   = targetObj;
    while (cont = cont.parentNode) {
        posObj.x+= cont.offsetLeft;
        posObj.y+= cont.offsetTop;
        if (cont.hasAttribute("id")) {
            break;
            }
        }
    targetObj.onmouseout = o2jse.menu.closeByTime;
    o2jse.menu.addMenu("jxWinMenu", "Window");
    o2jse.menu.menuList["jxWinMenu"].addItem("J",
                                             "jxWinRefresh",
                                             "Refresh",
                                             function() {
                                                o2jse.cmd.submit(targetObj.o2.e);
                                                });
    o2jse.menu.menuList["jxWinMenu"].addItem("S");
    o2jse.menu.menuList["jxWinMenu"].addItem("J",
                                             "jxWinMaxRes",
                                             (targetObj.o2.max ? "Restore" : "Maximize"),
                                             function() {
                                                o2jse.win.maxRest(targetObj);
                                                });

    var form_in = document.getElementById(targetObj.o2.f + "_" + targetObj.o2.e + "_in");
    var btn     = document.getElementById(targetObj.o2.f + "_" + targetObj.o2.e + "_btn");
    o2jse.menu.menuList["jxWinMenu"].addItem("J",
                                             "jxWinOpenClose",
                                             (form_in.style.display == 'none' ?
                                              "Show" : "Minimize"),
                                             btn.onclick);
    o2jse.menu.menuList["jxWinMenu"].addItem("S");
    o2jse.menu.menuList["jxWinMenu"].addItem("J",
                                             "jxWinExit",
                                             "Exit",
                                             function() {
                                                o2jse.win.exit(targetObj);
                                                });

    o2jse.menu.menuList["jxWinMenu"].show(posObj, null, targetObj);
    o2jse.win.showMenu = true;

    };


/**
 * Exit the window
 *
 * @param object targetObj   Object on which event is fired
 */
o2jse.win.exit = function(targetObj) {

    o2jse.ctrl.init(targetObj);
    o2jse.ctrl.make_waiting(targetObj);
    var objInfo  = targetObj.o2;
    var winDiv   = document.getElementById(objInfo.f + "_" + objInfo.e);
    var cmdField = o2jse.infoForm[objInfo.f + objInfo.e + "_jxcmd"];
    o2jse.ctrl.init(winDiv);
    if (winDiv.o2.exit) {
        cmdField.value = "1";
        o2jse.infoForm['o2_modfields'].value+= cmdField.name + ";";
        o2jse.infoForm['o2lastform'].value   = objInfo.f;
        o2jse.infoForm['o2lastctrl'].value   = "";
        if (o2jse.cliMode) {
            jxjs.request();
            }
        else {
            o2jse.cmd.submit(objInfo.e);
            }
        }

    };


/**
 * Insert window in list of windows needing relocation
 *
 * @param {String} win_id   Window object id
 */
o2jse.win.needRepos = function(win_id) {

    o2jse.reposWins[o2jse.reposWins.length] = win_id;

    };


/**
 * Delete window from list of windows needing relocation
 *
 * @param {String} win_id   Window object id
 */
o2jse.win.needNoRepos = function(win_id) {

    for (var i = (o2jse.reposWins.length - 1); i >= 0; i--) {
        if (o2jse.reposWins[i] == win_id) {
            o2jse.reposWins.splice(i, 1);
            break;
            }
        }

    };


/**
 * Relocate window after login or after resize
 *
 * @param string win_id   Window object id
 */
o2jse.win.repos = function(win_id) {

    var winDiv = null;
    if (!(winDiv = document.getElementById(win_id))) {
        return;
        }
    o2jse.ctrl.init(winDiv);
    var objInfo    = winDiv.o2;
    var objPos     = winDiv.style;
    var marginTop  = 0;
    var marginLeft = 0;
    if (o2jse.menu.appMainMenu) {
        if (o2jse.menuStyle == 'T') {
            marginTop = (document.getElementById("jxMenuBar") ?
                         document.getElementById("jxMenuBar").offsetHeight : 0);
            }
        else if (o2jse.menuStyle == 'L') {
            marginLeft = (document.getElementById("jxMenuBar") ?
                          document.getElementById("jxMenuBar").offsetWidth : 0);
            }
        }
    // __________________________________________________________ Horizontal alignment ___
    switch (objInfo.alignH) {
        case "center":
            objPos.left = Math.max(0, parseInt((o2jse.cli.width / 2) -
                                               (objInfo.x || 0) + marginLeft / 2)) + "px";
            break;
        case "right":
            objPos.right = (objInfo.x || 0) + "px";
            break;
        default:
            objPos.left = Math.max(0, (objInfo.x || 0)) + "px";
            break;
        }
    // ____________________________________________________________ Vertical alignment ___
    switch (objInfo.alignV) {
        case "middle":
            objPos.top = Math.max(0, parseInt((o2jse.cli.height / 2) -
                                              (objInfo.y || 0) + marginTop / 2)) + "px";
            break;
        case "bottom":
            objPos.bottom = (objInfo.y || 0) + "px";
            break;
        default:
            objPos.top = Math.max(0, parseInt(objInfo.y || 0)) + "px";
            break;
        }
    // _____________________________________________________________ Repos sub-windows ___
    if (objInfo.subf) {
        for(var idx = objInfo.subf; idx > 0; idx--) {
            var subF = null;
            if (subF = document.getElementById(objInfo.f + "_" + objInfo.e + idx)) {
                o2jse.ctrl.init(subF);
                subF.style.left = (winDiv.offsetLeft + subF.o2.x) + "px";
                subF.style.top  = (winDiv.offsetTop + subF.o2.y) + "px";
                }
            }
        }

    };


/**
 * Lookup control related functions collection
 *
 */
o2jse.lu = {

    listObj    : null,   /* Last active list (see descField.listObj)                    */
    reqDelay   : 300,    /* Time after which request is fired while typing              */
    openTimer  : null,   /* Timeout object for delayed popup                            */
    closeTimer : null    /* Timeout object for items list closing                       */

    };


/**
 * Handler for key entry events (onKeyDown) on lookup control.
 * This same method is applied to edit field (description) and to items list.
 *
 * @param object eventObj    Event object fired on lookup control
 * @param object targetObj   Lookup editable INPUT field
 */
o2jse.lu.k = function(eventObj, targetObj) {

    var stdEvent  = o2jse.event.std(eventObj);
    var KEY_ENTER = 13;
    var KEY_ESC   = 27;
    var KEY_TAB   = 9;
    var KEY_UP    = 38;
    var KEY_DOWN  = 40;
    var KEY_F1    = 112;
    var KEY_F12   = 123;
    var KEY_SHIFT = 16;
    var KEY_ALT   = 18;
    var KEY_PGUP  = 33;
    var KEY_PGDN  = 34;
    var doMove    = false;
    var seleItem;
    // _____________________________________________________________ * Standard keys * ___
    if (!stdEvent.ctrlKey              &&
        !stdEvent.altKey               &&
        stdEvent.keyCode != KEY_ENTER  &&
        stdEvent.keyCode != KEY_ESC    &&
        stdEvent.keyCode != KEY_TAB    &&
        (stdEvent.keyCode < KEY_PGUP   ||
         stdEvent.keyCode > KEY_DOWN)  &&
        (stdEvent.keyCode < KEY_F1     ||
         stdEvent.keyCode > KEY_F12)   &&
        (stdEvent.keyCode < KEY_SHIFT  ||
         stdEvent.keyCode > KEY_ALT)) {
        o2jse.lu.list(targetObj, false, false, stdEvent);
        targetObj.inEdit = true;
        }
    // _____________________________________________________________________ * KeyUp * ___
    else if (stdEvent.keyCode == KEY_UP) {
        // ________________________________________________ Event fired on input field ___
        if (targetObj.nodeName.toLowerCase() == "input") {
            // _______________________________________________________ If list is open ___
            if (targetObj.listObj) {
                stdEvent.stop();
                return true;
                }
            }
        // _________________________________________________ Event fired on items list ___
        else {
            var itemsDivs = targetObj.getElementsByTagName("div");
            var seleId    = parseInt(targetObj.jxSeleId);
            var codeVal   = "";
            if (seleId > 0) {
                var cI = itemsDivs[seleId];
                var nI = itemsDivs[seleId - 1];
                if (cI.className.indexOf("jxlsr")) {
                    cI.className = cI.className.substr(0, cI.className.indexOf("jxlsr"));
                    }
                nI.className      += " jxlsr";
                codeVal            = nI.id.substr(7);
                seleItem           = nI;
                doMove             = true;
                targetObj.jxSeleId = (seleId - 1);
                }
            }
        }
    // ___________________________________________________________________ * KeyDown * ___
    else if (stdEvent.keyCode == KEY_DOWN) {
        // ________________________________________________ Event fired on input field ___
        if (targetObj.nodeName.toLowerCase() == "input") {
            // _____________________________ ALT + KeyDown (reserved to open the list) ___
            if (stdEvent.altKey && !targetObj.listObj) {
                o2jse.lu.list(targetObj, true, true);
                }
            // ____________________________________ If list is open move down on items ___
            else if (targetObj.listObj) {
                targetObj.inEdit = false;
                var list         = targetObj.listObj.childNodes[0];
                list.jxSeleId    = -1;
                var itemsDivs    = list.getElementsByTagName("div");
                var codeVal      = '';
                if (itemsDivs.length > 0) {
                    var nI = itemsDivs[0];
                    nI.className += " jxlsr";
                    codeVal       = nI.id.substr(7);
                    seleItem      = nI;
                    doMove        = true;
                    list.jxSeleId = 0;
                    targetObj     = list;
                    list.focus();
                    }
                stdEvent.stop();
                }
            }
        // _________________________________________________ Event fired on items list ___
        else {
            var itemsDivs = targetObj.getElementsByTagName("div");
            var seleId    = parseInt(targetObj.jxSeleId);
            var codeVal   = "";
            if (seleId < (itemsDivs.length - 1)) {
                var cI = itemsDivs[seleId];
                var nI = itemsDivs[seleId + 1];
                if (seleId > -1 && cI.className.indexOf("jxlsr")) {
                    cI.className = cI.className.substr(0,
                                                       cI.className.indexOf("jxlsr"));
                    }
                nI.className      += " jxlsr";
                codeVal            = nI.id.substr(7);
                seleItem           = nI;
                doMove             = true;
                targetObj.jxSeleId = (seleId + 1);
                }
            }
        }
    // ____________________________________________________________________ * PageUp * ___
    else if (stdEvent.keyCode == KEY_PGUP) {
        if (targetObj.nodeName.toLowerCase() != "input") {
            var itemsDivs = targetObj.getElementsByTagName("div");
            var seleId    = parseInt(targetObj.jxSeleId);
            var codeVal   = "";
            var cI        = itemsDivs[seleId];
            var nI        = itemsDivs[0];
            if (seleId > -1 && cI.className.indexOf("jxlsr")) {
                cI.className = cI.className.substr(0, cI.className.indexOf("jxlsr"));
                }
            nI.className      += " jxlsr";
            codeVal            = nI.id.substr(7);
            seleItem           = nI;
            doMove             = true;
            targetObj.jxSeleId = (itemsDivs.length - 1);
            }
        }
    // __________________________________________________________________ * PageDown * ___
    else if (stdEvent.keyCode == KEY_PGDN) {
        if (targetObj.nodeName.toLowerCase() != "input") {
            var itemsDivs = targetObj.getElementsByTagName("div");
            var seleId    = parseInt(targetObj.jxSeleId);
            var codeVal   = "";
            var cI        = itemsDivs[seleId];
            var nI        = itemsDivs[itemsDivs.length - 1];
            if (cI.className.indexOf("jxlsr")) {
                cI.className = cI.className.substr(0, cI.className.indexOf("jxlsr"));
                }
            nI.className      += " jxlsr";
            codeVal            = nI.id.substr(7);
            seleItem           = nI;
            doMove             = true;
            targetObj.jxSeleId = 0;
            }
        }
    // _____________________________________________________________________ * ENTER * ___
    else if (stdEvent.keyCode == KEY_ENTER) {
        // _________________________________________________ Event fired on desc field ___
        if (targetObj.nodeName.toLowerCase() == "input") {
            if (targetObj.listObj) {
                var list = targetObj.listObj.childNodes[0].childNodes;
                // _______________________________________ Select first element if any ___
                if (list.length > 0) {
                    o2jse.lu.m(eventObj, list[0]);
                    }
                }
            targetObj.inEdit = false;
            }
        // _______________________________________________________ Event fired on list ___
        else {
            targetObj.descField.focus();
            o2jse.lu.m(eventObj, targetObj.selectedItem, true);
            }
        }
    // _______________________________________________________________________ * TAB * ___
    else if (stdEvent.keyCode == KEY_TAB) {
        // _____________________________________________ TAB event fired on desc field ___
        if (targetObj.nodeName.toLowerCase() == "input") {
            if (targetObj.listObj) {
                var list = targetObj.listObj.childNodes[0].childNodes;
                // ____________________________________ If waiting for a list response ___
                if (o2jse.waitObj || targetObj.pastedValue) {
                    targetObj.tabbedValue = targetObj.value.trim();
                    targetObj.listObj.style.display = 'none';
                    return false;
                    }
                // ______________________________________ If description field blanked ___
                else if ((targetObj.value.trim() == '' &&
                          list[0] &&
                          list[0].innerText.trim() == '') ||
                         // ________________ ... or select element if just one in list ___
                         (list.length == 1)) {
                    o2jse.lu.m(eventObj, list[0], true);
                    }
                else if (targetObj.listObj) {
                    o2jse.lu.e(targetObj, stdEvent);
                    }
                }
            }
        // ___________________________________________________ TAB event fired on list ___
        else {
            // ___________________________ NOTE: This should be the correct behaviour! ___
            // o2jse.lu.e(targetObj, stdEvent);
            var nF;
            // ___________________________________________________ Preserve focus flow ___
            nF = o2jse.createInput(o2jse.infoForm, "hidden", "", "1", "jxnofocus");
            o2jse.submitCtrl = targetObj.o2.c + targetObj.o2.e;
            o2jse.lu.m(eventObj, targetObj.selectedItem);
            // _____________________________________________ Remove no-focus directive ___
            if (nF) {
                o2jse.removeEl(nF);
                }
            }
        }
    // _______________________________________________________________________ * ESC * ___
    else if (stdEvent.keyCode == KEY_ESC) {
        if (targetObj.nodeName.toLowerCase() == "input") {
            if (targetObj.listObj) {
                o2jse.lu.e(targetObj, stdEvent);
                stdEvent.stop();
                targetObj.inEdit = false;
                return true;
                }
            }
        else {
            o2jse.lu.e(targetObj.descField, stdEvent);
            stdEvent.stop();
            targetObj.descField.inEdit = false;
            return true;
            }
        }
    // __________________________________________________ Process moving on items list ___
    if (doMove) {
        stdEvent.stop();
        o2jse.ctrl.init(targetObj);
        targetObj.selectedItem = seleItem;
        var o2data             = targetObj.o2;
        var codeField          = o2jse.infoForm[o2data.c + o2data.e];
        codeField.value        = codeVal;
        codeField.o2           = o2data;
        // ___________________________________________ Ensure selected item visibility ___
        o2jse.showByScroll(seleItem);
        return true;
        }
    return false;

    };


/**
 * Handler for OnFocus events on lookup control
 *
 * @param object targetObj   Object on which event is fired
 */
o2jse.lu.f = function(targetObj) {

    o2jse.ctrl.init(targetObj);
    var infoObj                        = targetObj.o2;
    o2jse.infoForm['o2lastform'].value = infoObj.f;
    o2jse.infoForm['o2lastctrl'].value = infoObj.c;
    if (o2jse.lu.closeTimer &&
        (targetObj.nodeName.toLowerCase() != "input" || targetObj.listObj)) {
        clearTimeout(o2jse.lu.closeTimer);
        }
    // ____________________________________________________ Event fired on input field ___
    if (targetObj.nodeName.toLowerCase() == "input") {
//        ATTENTION: focus class is defined as class:hover, because changing the
//        className here breaks onClick events!
//        targetObj.className = infoObj.cssf + "_focus" + (infoObj.z ? " jxzoom" : "") +
//                                                        (infoObj.fret ? " jxsoc" : "");
        // __________________________________________________________ Reset save-value ___
        if (typeof targetObj.saveValue == "undefined") {
            targetObj.saveValue = targetObj.value;
            }
        // ________________________________________________________ Reset TABbed value ___
        targetObj.tabbedValue = false;
        targetObj.pastedValue = false;
        }

    };


/**
 * Handler for on-paste events on lookup control.
 * This method is applied to lookup edit field (description).
 *
 * @param object targetObj   Lookup editable INPUT field
 */
o2jse.lu.p = function(targetObj) {

    targetObj.inEdit      = true;
    targetObj.pastedValue = true;
    setTimeout(function() {
                    o2jse.lu.list(targetObj, false, true);
                    }, 0);

    };


/**
 * Handler for blur events on lookup control
 *
 * @param object targetObj   Lookup editable INPUT field
 */
o2jse.lu.b = function(targetObj) {

    o2jse.ctrl.init(targetObj);
    var descField;
    // ___________________________________________________________ Blur on INPUT field ___
    if (targetObj.nodeName.toLowerCase() == "input") {
        descField = targetObj;
        }
    // __________________________________________________________________ Blur on list ___
    else {
        descField = targetObj.descField;
        }
    descField.pastedValue = false;
    if (descField.tabbedValue) {
        // _____ If submit-on-change stop all and get code from descriptin server-side ___
        if (descField.o2.fret) {
            descField.inEdit           = false;
            descField.stopForSelection = true;
            descField.saveValue        = descField.value;
            descField.tabbedValue      = false;
            codeField                  = o2jse.infoForm[descField.o2.c + descField.o2.e];
            o2jse.lu.listOff(descField);
            // ___________________________________________________ Preserve focus flow ___
            nF = o2jse.createInput(o2jse.infoForm, 'hidden', '', '1', 'jxnofocus');
            // __________________________________ Force server-side description decode ___
            dF = o2jse.createInput(o2jse.infoForm, 'hidden', '', descField.value,
                                   descField.o2.c + descField.o2.e + '_force_decode');
            if (o2jse.cliMode) {
                o2jse.ctrl.make_waiting(descField);
                jxjs.request(codeField, codeField.value);
                }
            else {
                o2jse.cmd.ctrlUpd(codeField);
                }
            // _____________________________________________ Remove no-focus directive ___
            if (nF) {
                o2jse.removeEl(nF);
                }
            // _____________________________ Remove force description decode directive ___
            if (dF) {
                o2jse.removeEl(dF);
                }
            }
        else {
            if (o2jse.lu.closeTimer) {
                clearTimeout(o2jse.lu.closeTimer);
                }
            o2jse.lu.listOff(descField);
            descField.tabbedValue = descField.value;
            o2jse.lu.list(descField, false, true);
            }
        }
    else {
        if (descField.listObj) {
            o2jse.lu.closeTimer = setTimeout(function () {
                                                o2jse.lu.listOff(descField); },
                                             100);
            }
        }
    descField.inEdit = false;

    };


/**
 * Handler for selection events on items list for combo-lookup
 *
 * @param {object}  eventObj   Event fired on list element
 * @param {object}  targetObj
 * @param {boolean} notFocus   If TRUE focus is not set to desc field
 */
o2jse.lu.m = function(eventObj, targetObj, notFocus) {

    var stdEvent = o2jse.event.std(eventObj);
    if (!targetObj) {
        targetObj = stdEvent.target;
        }
    var listObj = targetObj.parentNode;
    var o2data  = listObj.o2;
    // _________________________________________________________________ Previous page ___
    if (targetObj.value == "@luu") {
        if (!o2jse.waitObj) {
            o2jse.waitObj = o2jse.createEl(listObj, "DIV", "jx_inctrl_wait", "&nbsp;");
            }
        o2jse.lu.exeReq(listObj.descField, 2);
        }
    // _____________________________________________________________________ Next page ___
    else if (targetObj.value == "@lud") {
        if (!o2jse.waitObj) {
            o2jse.waitObj = o2jse.createEl(listObj, "DIV", "jx_inctrl_wait", "&nbsp;");
            }
        o2jse.lu.exeReq(listObj.descField, 1);
        }
    // ________________________________________________________________ Item selection ___
    else {
        var codeField       = o2jse.infoForm[o2data.c + o2data.e];
        var descField       = listObj.descField;
        codeField.value     = targetObj.value;
        descField.value     = targetObj.innerHTML.decode().rtrim();
        descField.saveValue = descField.value;
        // ___________________________________________ Stop later updates on selection ___
        descField.stopForSelection = true;
        // ___________________________________________ Reset "value not in list" error ___
        descField.className = descField.className.replace(' jxerror', '');
        // _________________________________ TAB events let focus flow to next control ___
        if (typeof notFocus === 'undefined') {
            descField.focus();
            }
        else {
            var nF;
            // ___________________________________________________ Preserve focus flow ___
            nF = o2jse.createInput(o2jse.infoForm, "hidden", "", "1", "jxnofocus");
            }
        descField.scrollTop  = 0;
        descField.scrollLeft = 0;
        o2jse.lu.listOff(descField);
        if (o2jse.cliMode && o2data.fret) {
            o2jse.ctrl.make_waiting(descField);
            jxjs.request(codeField, codeField.value);
            }
        else {
            o2jse.cmd.ctrlUpd(codeField);
            }
        // _________________________________________________ Remove no-focus directive ___
        if (nF) {
            o2jse.removeEl(nF);
            }
        }
    listObj.innerHTML = "";
    descField.inEdit  = false;

    };


/**
 * Handler for ESCAPE press events on lookup control (called by o2jse.lu.k)
 *
 * @param {object} targetObj   Lookup editable INPUT field
 * @param {object} eventObj    Event object for ESCAPE pressing
 */
o2jse.lu.e = function(targetObj, eventObj) {

    o2jse.ctrl.init(targetObj);
    var descElement;
    // ____________________________________________________ Event fired on input field ___
    if (targetObj.nodeName.toLowerCase() == "input") {
        descElement = targetObj;
        o2jse.lu.listOff(descElement);
        descElement.value = descElement.saveValue;
        if (descElement.select) {
            descElement.select();
            }
        }
    // ___________________________________________________________ Event fired on list ___
    else {
        descElement = targetObj.descField;
        o2jse.lu.listOff(descElement);
        descElement.focus();
        }
    descElement.name    = "";
    descElement.listObj = null;
    o2jse.lu.listObj    = null;

    };


/**
 * Handler for onMouseDown events on lookup control
 *
 * @param {object} targetObj   Lookup editable INPUT field receiving event
 */
o2jse.lu.ck = function(targetObj) {

    if (targetObj.listObj) {
        return o2jse.lu.listOff(targetObj);
        }
    else {
        // ________________________ Delay click execution to solve double-click events ___
        if (targetObj.getAttribute("jx-dblclick") == null) {
            targetObj.setAttribute("jx-dblclick", 1);
            setTimeout(function () {
                        // ___________ If flag is present (no re-click before timeout) ___
                        if (targetObj.getAttribute("jx-dblclick") == 1) {
                            o2jse.lu.list(targetObj, true, true);
                            }
                        targetObj.removeAttribute("jx-dblclick");
                        },
                       250);
            }
        // ______________________________________ Re-click before timeout: remove flag ___
        else {
            targetObj.removeAttribute("jx-dblclick");
            }
        if (targetObj.select) {
            targetObj.select();
            }
        }
    return false;

    };


/**
 * Compose and display items list panel
 *
 * @param {object}  targetObj   o2 lookup INPUT field
 * @param {boolean} complete    TRUE for complete list (discarding filter text)
 * @param {boolean} immediate   If action is to be executed now, without timeout
 */
o2jse.lu.list = function(targetObj, complete, immediate, stdEvent) {

    o2jse.ctrl.init(targetObj);
    var o2data   = targetObj.o2;
    var posLocal = o2jse.getPos(targetObj);
    var listElement;
    o2jse.infoForm['o2_prgexeid'].value = o2data.e;
    o2jse.infoForm['o2lastform'].value  = o2data.f;
    o2jse.infoForm['o2lastctrl'].value  = o2data.c;
    // _______________________________ For complete list filter field is not requested ___
    if (complete) {
        targetObj.name  = "";
        }
    else {
        targetObj.name = o2data.c + o2data.e + "_desc";
        }
    if (targetObj.listObj) {
        targetObj.listObj.style.display = 'block';
        o2jse.elBody.appendChild(targetObj.listObj);
        listElement           = targetObj.listObj.childNodes[0];
        listElement.innerHTML = "";
        }
    else {
        listElement                      = document.createElement("DIV");
        listElement.tabIndex             = 0;
        listElement.onkeydown            = function(eO) { o2jse.lu.k(eO, this); };
        listElement.onfocus              = function() { o2jse.lu.f(this); };
        listElement.onblur               = function() { o2jse.lu.b(this); };
        listElement.scrollTop            = 0;
        targetObj.listObj                = o2jse.createEl(o2jse.elBody, "DIV");
        targetObj.listObj.style.position = "absolute";
        targetObj.listObj.appendChild(listElement);
        }
    listElement.descField       = targetObj;
    listElement.o2              = o2data;
    listElement.className       = o2data.cssl;
    listElement.style.minWidth  = targetObj.offsetWidth + "px";
    listElement.style.minHeight = "50px";
    // _____________________________________________________________ Dynamic data list ___
    if (o2data.dyn) {
        if (!o2jse.waitObj) {
            o2jse.waitObj = o2jse.createEl(targetObj.listObj,
                                           "DIV", "jx_inctrl_wait", "&nbsp;");
            }
        if (o2jse.lu.openTimer) {
            clearTimeout(o2jse.lu.openTimer);
            }
        if (immediate) {
            o2jse.lu.exeReq(targetObj);
            }
        else {
            o2jse.lu.openTimer = setTimeout(function() { o2jse.lu.exeReq(targetObj); },
                                            o2jse.lu.reqDelay);
            }
        }
    // ______________________________________________________________ Static data list ___
    else {
        // _______________________________________________________ Manage actual value ___
        if (stdEvent) {
            // ____________________________________ Blank selected text when key-press ___
            if (targetObj.selectionEnd > targetObj.selectionStart) {
                targetObj.value = targetObj.value.substr(0, targetObj.selectionStart) +
                                  targetObj.value.substr(targetObj.selectionEnd);
                }
            var filterValue = targetObj.value.toLowerCase();
            if (stdEvent.keyCode == 8) {
                filterValue = filterValue.substr(0, filterValue.length - 1);
                }
            // ________________________________________________________ Underscore (_) ___
            else if ((stdEvent.keyCode == 173) || (stdEvent.keyCode == 189)) {
                filterValue = filterValue + '_';
                }
            else {
                filterValue = (filterValue + String.fromCharCode(stdEvent.keyCode)
                              ).toLowerCase().trim();
                }
            }
        var itemCode;
        var itemDesc;
        var optLocal;
        listElement.innerHTML = "";
        for (var elementId in o2data.items) {
            itemCode = o2data.items[elementId][0];
            itemDesc = o2data.items[elementId][1].trim();
            itemDesc = (itemDesc ? itemDesc : "&nbsp;");
            if (complete ||
                !filterValue ||
                itemDesc.toLowerCase().indexOf(filterValue) > -1) {
                optLocal         = o2jse.createEl(listElement, "DIV", "", itemDesc);
                optLocal.value   = itemCode;
                optLocal.onclick = function(e) { o2jse.lu.m(e); };
                }
            }
        }
    targetObj.listObj.style.left = posLocal.x + "px";
    // ________________________________________________________ Page vertical overflow ___
    if ((posLocal.y < (o2jse.cli.height -
                      targetObj.offsetHeight -
                      targetObj.listObj.offsetHeight)) ||
        (posLocal.y < targetObj.listObj.offsetHeight)) {
        targetObj.listObj.style.top = (posLocal.y + targetObj.offsetHeight) + "px";
        }
    else {
        targetObj.listObj.style.top = (posLocal.y - targetObj.listObj.offsetHeight) +"px";
        }
    o2jse.lu.listObj           = targetObj.listObj;
    // __________________________________ Reset updates for selection on new list open ___
    targetObj.stopForSelection = false;

    };


/**
 * Close items list
 *
 * @param {Object} descField   Combo description field after blur event
 */
o2jse.lu.listOff = function(descField) {

    if (o2jse.lu.closeTimer) {
        clearTimeout(o2jse.lu.closeTimer);
        }
    var targetObj;
    if (descField) {
        targetObj = descField;
        }
    else if (o2jse.lu.listObj) {
        targetObj        = o2jse.lu.listObj.childNodes[0].descField;
        o2jse.lu.listObj = null;
        }
    else {
        return false;
        }
    if (targetObj.tabbedValue === false) {
        if (targetObj.listObj) {
            o2jse.removeEl(targetObj.listObj);
            }
        targetObj.listObj = null;
        o2jse.lu.listObj  = null;
        if (descField) {
            descField.value = descField.saveValue;
            }
        }

    };


/**
 * Executes AJAX-style request for lookup items list. Returned page is recovered by
 * callback method o2jse.lu.getList().
 *
 * @param {Object}  descField   Input field requesting the list
 * @param {Integer} act         Action: 1 = Next page; 2 = Previous page; 0 (other) = List
 */
o2jse.lu.exeReq = function(descField, act) {

    var luAct = (act == 1 ? "lunextpg" : (act == 2 ? "luprevpg" : "lulist"));
    if (descField.listObj) {
        var firedCtrl   = (act == 1 || act == 2 ?
                           descField.listObj.childNodes[0] : descField);
        reqBody         = 'jxluact=' + luAct +
                          '&JXSESSNAME=' + o2jse.sessName +
                          '&o2_prgexeid=' + descField.o2.e +
                          '&o2lastform=' + descField.o2.f +
                          '&o2lastctrl=' + descField.o2.c +
                          '&' +
                          descField.name + '=' + encodeURIComponent(descField.value);
        firedCtrl.reqId = o2jse.requester.exe('lookup',
                                              reqBody,
                                              firedCtrl,
                                              o2jse.lu.getList,
                                              true);
        }

    };


/**
 * Manage lookup list AJAX-style receiving for mobile browsing. Page, requested by method
 * o2jse.ctrl.exeReq(), is recovered and analysed to get items lines.
 *
 * @param object ctrlObj    Control object (HTML element) which requested data
 * @param string listText   Returned page body text containing requested data
 * @param string reqId      Request unique ID
 */
o2jse.lu.getList = function(ctrlObj, listText, reqId) {

    // _____________________________________________________________ Remove wait-image ___
    if (o2jse.waitObj) {
        o2jse.removeEl(o2jse.waitObj);
        delete o2jse.waitObj;
        }
    if (jxjs.waitingCtrl) {
        jxjs.waitingCtrl.style.display = "block";
        }
    // ________ Get only last request response and stop list updates on selection done ___
    if (ctrlObj.reqId != reqId || ctrlObj.stopForSelection) {
        return false;
        }
    var itemsList;
    // _________________________________________________________ Fired from desc-field ___
    if (ctrlObj.listObj) {
        itemsList = ctrlObj.listObj.childNodes[0];
        }
    // _________________________________________________________ Fired from items list ___
    else if (ctrlObj.descField) {
        itemsList = ctrlObj;
        }
    else {
        return false;
        }
    itemsList.style.height = null;
    itemsList.innerHTML = "";
    itemsList.jxSeleId  = -1;
    if (listText) {
        var listObjLocal = {};
        var optLocal     = {};
        var itemTxt;
        eval("listObjLocal = " + listText + ";");
         // _______________________________________________________ Previous page link ___
        if (!listObjLocal.f) {
            optLocal         = o2jse.createEl(itemsList, "DIV", "", "\u00ab");
            optLocal.value   = "@luu";
            optLocal.onclick = function(e) { o2jse.lu.m(e); };
            optLocal.className = "o2lu_sysppg";
            }
        // _____________________________________________________________ Loop on items ___
        for (var singleItem in listObjLocal.v) {
            itemTxt          = listObjLocal.d[singleItem].trim();
            itemTxt          = (itemTxt ? itemTxt : "&nbsp;");
            optLocal         = o2jse.createEl(itemsList, "DIV", "", itemTxt);
            optLocal.value   = listObjLocal.v[singleItem];
            optLocal.onclick = function(e) { o2jse.lu.m(e); };
            }
        // ____________________________________________________________ Next page link ___
        if (!listObjLocal.l) {
            optLocal         = o2jse.createEl(itemsList, "DIV", "", "\u00bb");
            optLocal.value   = "@lud";
            optLocal.onclick = function(e) { o2jse.lu.m(e); };
            optLocal.className = "o2lu_sysnpg";
            }
        }
    // ______________________________________________________ If event fired from list ___
    if (itemsList == ctrlObj) {
        ctrlObj.jxSeleId = -1;
        var itemsDivs = ctrlObj.getElementsByTagName("div");
        var codeVal   = "";
        // __________________________________________________ Select first item if any ___
        if (itemsDivs.length > 0) {
            var nI = itemsDivs[0];
            nI.className += " jxlsr";
            codeVal       = nI.id.substr(7);
            seleItem      = nI;
            ctrlObj.jxSeleId = 0;
            ctrlObj.focus();
            }
        ctrlObj.selectedItem = seleItem;
        var o2data           = ctrlObj.o2;
        var codeField        = o2jse.infoForm[o2data.c + o2data.e];
        codeField.value      = codeVal;
        codeField.o2         = o2data;
        // ___________________________________________ Ensure selected item visibility ___
        o2jse.showByScroll(seleItem);
        }
    // _____________________________________ Event fired on desc field and TABbed away ___
    else if (ctrlObj.tabbedValue !== false) {
        if ((ctrlObj.tabbedValue == '' &&
             itemsList.childNodes.length > 0 &&
             itemsList.childNodes[0].innerText.trim() == '') ||
             // ____________________________ ... or select element if just one in list ___
             (itemsList.childNodes.length == 1)) {
            ctrlObj.tabbedValue = false;
            o2jse.lu.m(null, itemsList.childNodes[0], true);
            }
        else {
            ctrlObj.tabbedValue = false;
            o2jse.lu.listOff(ctrlObj);
            }
        }

    };


/**
 * Listbox control related functions collection
 */
o2jse.lb = {};


/**
 * Handler for click selection events on items list in mobile browsing
 *
 * @param {object} eventObj   Event fired on lookup and listbox list
 */
o2jse.lb.m = function(eventObj) {

    var stdEvent  = o2jse.event.std(eventObj);
    var targetObj = stdEvent.target;
    // ____________________________________ Filter clicks on items only, not container ___
    if (!targetObj.onclick) {
        var tarParent = targetObj.parentNode;
        var itemsDivs = tarParent.getElementsByTagName("div");
        for (var i = itemsDivs.length - 1; i >= 0; i--) {
            itemsDivs[i].className = "";
            }
        targetObj.className = "jxlsr";
        o2jse.ctrl.init(tarParent);
        var o2data      = targetObj.parentNode.o2;
        var codeField   = o2jse.infoForm[o2data.c + o2data.e];
        codeField.value = targetObj.id.substr(7);
        codeField.o2    = o2data;
        if (o2jse.cliMode && o2data.fret) {
            jxjs.request(codeField, codeField.value);
            }
        else {
            o2jse.cmd.ctrlUpd(codeField);
            }
        }

    };


/**
 * Handler for keyboard events on items list in mobile browsing
 *
 * @param {object} eventObj   Event fired on lookup and listbox list
 */
o2jse.lb.k = function(eventObj) {

    var stdEvent  = o2jse.event.std(eventObj);
    var targetObj = stdEvent.target;
    var doMove    = false;
    var seleItem;
    // _________________________________________________________________________ KeyUp ___
    if (stdEvent.keyCode == 38) {
        var itemsDivs = targetObj.getElementsByTagName("div");
        var seleId    = parseInt(targetObj.jxSeleId || 0);
        var codeVal   = "";
        if (seleId > 0) {
            var cI = itemsDivs[seleId];
            var nI = itemsDivs[seleId - 1];
            if (cI.className.indexOf("jxlsr")) {
                cI.className = cI.className.substr(0, cI.className.indexOf("jxlsr"));
                }
            nI.className      += " jxlsr";
            codeVal            = nI.id.substr(7);
            seleItem           = nI;
            doMove             = true;
            targetObj.jxSeleId = (seleId - 1);
            }
        }
    // _______________________________________________________________________ KeyDown ___
    if (stdEvent.keyCode == 40) {
        var itemsDivs = targetObj.getElementsByTagName("div");
        var seleId    = parseInt(targetObj.jxSeleId || 0);
        var codeVal   = "";
        if (seleId < (itemsDivs.length - 1)) {
            var cI = itemsDivs[seleId];
            var nI = itemsDivs[seleId + 1];
            if (seleId > -1 && cI.className.indexOf("jxlsr")) {
                cI.className = cI.className.substr(0, cI.className.indexOf("jxlsr"));
                }
            nI.className      += " jxlsr";
            codeVal            = nI.id.substr(7);
            seleItem           = nI;
            doMove             = true;
            targetObj.jxSeleId = (seleId + 1);
            }
        }
    // _______________________________________________________________ Process control ___
    if (doMove) {
        stdEvent.stop();
        o2jse.ctrl.init(targetObj);
        var o2data      = targetObj.o2;
        var codeField   = o2jse.infoForm[o2data.c + o2data.e];
        codeField.value = codeVal;
        codeField.o2    = o2data;
        // ___________________________________________ Ensure selected item visibility ___
        o2jse.showByScroll(seleItem);
        if (o2jse.cliMode && o2data.fret) {
            jxjs.request(codeField, codeField.value);
            }
        else {
            o2jse.cmd.ctrlUpd(codeField);
            }
        }

    };


/**
 * File upload control related functions collection
 */
o2jse.fu = {};


/**
 * Clear control selection
 *
 * @param {String} ctrlName   Upload control name
 */
o2jse.fu.clear = function(ctrlName) {

    var ctrlObj = o2jse.infoForm[ctrlName];
    ctrlObj.outerHTML = ctrlObj.outerHTML;
    document.getElementById(ctrlName + "_int").innerHTML = "";

    };


/**
 * Manage on-change events on control
 *
 * @param {String} ctrlName   Upload control name
 */
o2jse.fu.c = function(ctrlName) {

    var txt = o2jse.infoForm[ctrlName].value;
    // ____________________________________________ Strip path and show only file name ___
    document.getElementById(ctrlName + "_int").innerHTML = txt.replace(/^.*[\\\/]/, '');
    o2jse.ctrl.upLoad = true;

    };


/**
 * Manage on-keydown events on control
 *
 * @param {Object} eventObj   Event fired on control
 * @param {Object} ctrlObj    Control on which key pressed
 */
o2jse.fu.k = function(eventObj, ctrlObj) {

    var stdEvent = o2jse.event.std(eventObj);
    // ______________________________________________________________ CANC e BackSpace ___
    if (stdEvent.keyCode == 46 || stdEvent.keyCode == 8) {
        ctrlName = ctrlObj.name;
        o2jse.fu.clear(ctrlName);
        o2jse.infoForm[ctrlName].focus();
        }

    };


/**
 * Rich Text Editor related functions collection
 */
o2jse.rte = {};


/**
 * Execute command on clicking on formatting buttons in wysiwyg editor area
 *
 */
o2jse.rte.btnExe = function(editAreaID, cmdStr) {

    document.execCommand(cmdStr, false, null);
    o2jse.rte.save(editAreaID);

    };


/**
 * Handler for OnBlur events on edit and text fields
 *
 * @param object targetObj   Object on which event is fired
 */
o2jse.rte.b = function(targetObj) {

    targetObj.contenteditable = false;
    var objInfo               = targetObj.o2;
    o2jse.rte.save(objInfo.c + objInfo.e);

    };


/**
 * Retrieves HTML content from a wysiwyg editor area and save it to field
 *
 */
o2jse.rte.save = function(editAreaID) {

    var divObj    = document.getElementById(editAreaID + "_edit");
    var divText   = divObj.innerHTML;
    var targetObj = o2jse.infoForm[editAreaID];
    if (targetObj.value !== divText) {
        targetObj.value = divText;
        // ______ This replaces o2jse.cmd.ctrlUpd method to place control at beginning ___
        var objInfo = divObj.o2;
        if (String(objInfo.msg) == "" || o2jse.cmd.confirm(objInfo.msg)) {
            var modFields = o2jse.infoForm['o2_modfields'];
            if (modFields.value.indexOf(objInfo.c + ";") < 0) {
                modFields.value = objInfo.c + ";" + modFields.value;
                }
            if (objInfo.fret) {
                o2jse.cmd.submit(objInfo.e);
                }
            }
        }

    };


/**
 * Check-box related functions
 *
 */
o2jse.cb = {};


/**
 * Handler for OnClick events on check-box controls
 *
 * @param {Object}  targetObj   Object on which event is fired
 * @param {Boolean} fRet        Force submit/request
 */
o2jse.cb.c = function(targetObj, fRet) {

    o2jse.ctrl.init(targetObj);
    var o2Info = targetObj.o2;
    if (!targetObj.hasAttribute("name")) {
        var namedObj = null;
        if (namedObj = document.getElementsByName(o2Info.c + o2Info.e)[0]) {
            namedObj.removeAttribute("name");
            }
        targetObj.name = o2Info.c + o2Info.e;
        }
    targetObj.value = (targetObj.checked ? "1" : "");
    if (fRet && o2jse.cliMode) {
        jxjs.request(targetObj, targetObj.value);
        }
    else {
        targetObj.o2.fret = fRet;
        o2jse.cmd.ctrlUpd(targetObj);
        }

    };


/**
 * Popup related functions collection
 *
 */
o2jse.pu = {

    ctrlObj : null,   /* Object on which popup is fired                                 */
    pos     : [0, 0], /* Event position (x, y)                                          */
    timer   : null,   /* Timeout for delayed popup                                      */
    outBox  : null,   /* Container (DIV) for whole popup                                */
    steps   : []      /* List of activated timeouts for fadeout                         */

    };


/**
 * Handler for OnOver events on controls
 *
 * @param object targetObj   Object on which event is fired
 */
o2jse.pu.i = function(eventObj) {

    var stdEvent = o2jse.event.std(eventObj);
    o2jse.ctrl.init(stdEvent.target);
    if (o2jse.pu.timer) {
        clearTimeout(o2jse.pu.timer);
        }
    // ____________________________________ Don't show popup if waiting for a response ___
    if (o2jse.submitting) {
        stdEvent.stop();
        return;
        }
    if (stdEvent.target.o2.e != document.forms.o2form['o2_prgexeid'].value) {
        stdEvent.stop();
        return;
        }
    if (!o2jse.pu.outBox || o2jse.pu.ctrlObj != stdEvent.target) {
        o2jse.pu.hide();
        o2jse.pu.ctrlObj = stdEvent.target;
        o2jse.pu.pos     = [stdEvent.x, stdEvent.y];
        o2jse.pu.timer   = setTimeout(o2jse.pu.show, 1000);
        }
    else {
        o2jse.pu.unhide();
        }

    };


/**
 * Handler for OnMouseOut events on controls
 *
 */
o2jse.pu.o = function() {

    if (o2jse.pu.timer) {
        clearTimeout(o2jse.pu.timer);
        }
    o2jse.pu.fadeOut();

    };


/**
 * Show popup on controls
 *
 */
o2jse.pu.show = function() {

    o2jse.pu.hide();
    var pos  = o2jse.getPos(o2jse.pu.ctrlObj);
    var ctrl = o2jse.pu.ctrlObj;
    var sele = "";
    var act  = "";
    o2jse.ctrl.init(ctrl);
    // _______________________________________________________________________ Out box ___
    o2jse.pu.outBox = o2jse.createEl(o2jse.elBody, "div", "jxpopup");
    o2jse.pu.outBox.style.position = "absolute";
    o2jse.pu.outBox.onmouseover    = o2jse.pu.unhide;
    o2jse.pu.outBox.onmouseout     = o2jse.pu.fadeOut;
    // ______________________________________________________ Control related position ___
    o2jse.pu.outBox.style.left     = o2jse.pu.pos[0] + "px";
    o2jse.pu.outBox.style.top      = o2jse.pu.pos[1] + "px";
    /*
    // ______________________________________________________ Control related position ___
    o2jse.pu.outBox.style.top      = pos.y + "px";
    o2jse.pu.outBox.style.left     = (pos.x + ctrl.offsetWidth) + "px";
    */
    if (!o2jse.waitObj) {
        o2jse.waitObj = o2jse.createEl(o2jse.pu.outBox, "DIV", "jx_inctrl_wait");
        o2jse.waitObj.style.top  = "5px";
        o2jse.waitObj.style.left = "5px";
        }
    // __________________________________________________________ If control is in tab ___
    if (ctrl.o2.pT == "tab") {
        // ________________________________________________________ Get grid selection ___
        var tr = o2jse.getParentTag(ctrl, "tr");
        o2jse.ctrl.init(tr);
        // ________________________________ Set new view selection for the environment ___
        sele = "&" + tr.o2.v + "_vs" + tr.o2.e + "=" +parseInt(tr.rowIndex / tr.o2.lines);
        }
    // _______________________________________________________ Set action before popup ___
    if (ctrl.o2.puact) {
        act = "&o2_action" + "=" + ctrl.o2.puact;
        }
    // _____________________________________________________ Start request for content ___
    o2jse.requester.exe("popup",
                        "jxpuexp=" + ctrl.o2.puexp + act + sele,
                        o2jse.pu.outBox,
                        o2jse.pu.getCode);

    };


/**
 * Hide the popup box
 *
 */
o2jse.pu.hide = function() {

    for (var i = o2jse.pu.steps.length - 1; i >= 0; i--) {
        clearTimeout(o2jse.pu.steps[i]);
        }
    o2jse.pu.steps = [];
    if (o2jse.pu.outBox) {
        o2jse.removeEl(o2jse.pu.outBox);
        o2jse.pu.outBox = null;
        o2jse.pu.pos    = [];
        if (o2jse.waitObj) {
            o2jse.removeEl(o2jse.waitObj);
            delete o2jse.waitObj;
            }
        }

    };


/**
 * Stop hiding the popup box
 *
 */
o2jse.pu.unhide = function() {

    for (var i = o2jse.pu.steps.length - 1; i >= 0; i--) {
        clearTimeout(o2jse.pu.steps[i]);
        }
    o2jse.pu.steps = [];
    o2jse.pu.setOpacity(100);

    };


/**
 * Set opacity level for the popup box
 *
 * @param {Integer} opLevel
 */
o2jse.pu.setOpacity = function(opLevel) {

    if (o2jse.pu.outBox) {
        o2jse.cmd.opacity(o2jse.pu.outBox, opLevel);
        }

    };


o2jse.pu.fadeOut = function() {

    var timer = 0;
    for (var i = 100; i >= 1; i--) {
        o2jse.pu.steps[i] = setTimeout("o2jse.pu.setOpacity("+i+")", timer * 7);
        timer++;
        }
    o2jse.pu.steps[o2jse.pu.steps.length] = setTimeout(o2jse.pu.hide, timer * 7);

    };


/**
 * Manage popup content AJAX-style receiving. Page, requested by method
 * o2jse.ctrl.exeReq(), is recovered and analysed to get HTML code.
 *
 * @param object ctrlObj    Control object (HTML element) which requested data
 * @param string contCode   Returned page body text containing requested data
 */
o2jse.pu.getCode = function(ctrlObj, contCode) {

    if (o2jse.waitObj) {
        o2jse.removeEl(o2jse.waitObj);
        delete o2jse.waitObj;
        }
    if (contCode) {
        ctrlObj.innerHTML = contCode;
        }
    else {
        o2jse.removeEl(ctrlObj);
        delete ctrlObj;
        }

    };


/**
 * Treeview related functions collection
 *
 */
o2jse.tv = {};


/**
 * Manage click on icon events in treeview for fold/unfold
 *
 * @param string target   Treeview node object that fired the event
 * @param string ctrl     Treeview control name
 * @param string node     Node id to be open/close
 */
o2jse.tv.c = function(target, ctrl, node) {

    o2jse.ctrl.init(target);
    var o2Info                         = target.o2;
    o2jse.infoForm['o2lastform'].value = o2Info.f;
    o2jse.infoForm['o2lastctrl'].value = o2Info.c;
    o2jse.ctrl.make_waiting(target);
    var treeCtrl = o2jse.createInput(o2jse.infoForm,
                                     "hidden",
                                     "",
                                     ctrl,
                                     "jxtree");
    var nodeCtrl = o2jse.createInput(o2jse.infoForm,
                                     "hidden",
                                     "",
                                     node,
                                     "jxtreefold");
    if (o2jse.cliMode) {
        jxjs.request();
        }
    else {
        o2jse.cmd.submit();
        }
    o2jse.removeEl(treeCtrl);
    o2jse.removeEl(nodeCtrl);

    };


/**
 * Manage click on item events in treeview for activate
 *
 * @param string TreeObj   Treeview control node object that fired the event
 * @param string node      Node id to be activated
 */
o2jse.tv.a = function(TreeObj, node) {

    o2jse.ctrl.init(TreeObj);
    var o2Info = TreeObj.o2;
    // ______________________________________________________________ Standard control ___
    if (o2Info.std) {
        o2jse.infoForm['o2lastform'].value = o2Info.f;
        o2jse.infoForm['o2lastctrl'].value = o2Info.c;
        var stdCtrl                        = o2jse.createInput(o2jse.infoForm,
                                                               false,
                                                               false,
                                                               node,
                                                               o2Info.c + o2Info.e);
        stdCtrl.o2                         = o2Info;
        if (o2jse.cliMode) {
            jxjs.request(stdCtrl, node);
            o2jse.removeEl(stdCtrl);
            }
        }
    // ___________________________________________ Scripting defined control (old way) ___
    else {
        // ________________________________________ Override standard focus management ___
        var tvFocus  = o2jse.createInput(o2jse.infoForm,
                                         false,
                                         false,
                                         o2Info.c,
                                         "jxtvlastctrl");
        var treeCtrl = o2jse.createInput(o2jse.infoForm,
                                         false,
                                         false,
                                         o2Info.c,
                                         "jxtree");
        var nodeCtrl = o2jse.createInput(o2jse.infoForm,
                                         false,
                                         false,
                                         node,
                                         "jxtreenode");
        if (o2jse.cliMode) {
            jxjs.request();
            o2jse.removeEl(tvFocus);
            o2jse.removeEl(treeCtrl);
            o2jse.removeEl(nodeCtrl);
            }
        }
    if (!o2jse.cliMode) {
        o2jse.cmd.submit();
        }

    };


/**
 * Handler for key entry events (onKeyDown) on treeview control.
 *
 * @param {Object}  eventObj        Event object fired on treeview control
 * @param {Boolean} fromContainer   If TRUE method is called from whole container
 *                                  (no focus node)
 */
o2jse.tv.k = function(eventObj, fromContainer) {

    var stdEvent  = o2jse.event.std(eventObj);
    var KEY_ENTER = 13;
    var KEY_ESC   = 27;
    var KEY_TAB   = 9;
    var KEY_UP    = 38;
    var KEY_DOWN  = 40;
    var KEY_LEFT  = 37;
    var KEY_RIGHT = 39;
    var KEY_F1    = 112;
    var KEY_F12   = 123;
    var KEY_SHIFT = 16;
    var KEY_ALT   = 18;
    var KEY_PGUP  = 33;
    var KEY_PGDN  = 34;
    var targetObj = stdEvent.target;
    o2jse.ctrl.init(targetObj);
    var tvName = targetObj.o2.c;
    var tvRet  = targetObj.o2.fret;
    // _____________________________________________________________ * Standard keys * ___
    /*
    if (!stdEvent.ctrlKey              &&
        !stdEvent.altKey               &&
        stdEvent.keyCode != KEY_ENTER  &&
        stdEvent.keyCode != KEY_ESC    &&
        stdEvent.keyCode != KEY_TAB    &&
        (stdEvent.keyCode < KEY_PGUP   ||
         stdEvent.keyCode > KEY_DOWN)  &&
        (stdEvent.keyCode < KEY_F1     ||
         stdEvent.keyCode > KEY_F12)   &&
        (stdEvent.keyCode < KEY_SHIFT  ||
         stdEvent.keyCode > KEY_ALT)) {
        }
    */
    // ___________________________________ Mehod called from container (no focus node) ___
    if (fromContainer) {
        if (stdEvent.keyCode == KEY_DOWN || stdEvent.keyCode == KEY_RIGHT) {
            o2jse.tv.f(targetObj, targetObj.getElementsByTagName("div")[0]);
            }
        }
    // ************************************************************************ KeyUp ****
    else if (stdEvent.keyCode == KEY_UP) {
        // ______________________________________ MOVE TO FIRST ELEMENT UP CURRENT ONE ___
        o2jse.tv.f(targetObj.parentNode, targetObj.parentNode.previousSibling);
        }
    // *********************************************************************** KeyDown ***
    else if (stdEvent.keyCode == KEY_DOWN) {
        // ____________________________________ MOVE TO FIRST ELEMENT DOWN CURRENT ONE ___
        o2jse.tv.f(targetObj.parentNode, targetObj.parentNode.nextSibling);
        }
    // *********************************************************************** KeyLeft ***
    else if (stdEvent.keyCode == KEY_LEFT) {
        // ____________________ CLOSE CHILD TREE OR MOVE TO FIRST LEVEL UP CURRENT ONE ___
        cL      = targetObj.parentNode.getElementsByTagName("span").length;
        openBtn = targetObj.previousSibling.previousSibling;
        // _______________________________________________________ Close current level ___
        if (openBtn.className == "jx_treenode_open") {
            // ____________________________________ Override standard focus management ___
            var tvFocus = o2jse.createInput(o2jse.infoForm,
                                            false,
                                            false,
                                            targetObj.o2.c,
                                            "jxtvlastctrl");
            openBtn.click();
            o2jse.removeEl(tvFocus);
            }
        // __________________________________________________________ Move to level up ___
        else {
            var upEl = targetObj.parentNode;
            var pL   = cL;
            while (pL && pL >= cL) {
                upEl = upEl.previousSibling;
                pL   = upEl.getElementsByTagName("span").length;
                }
            // ____________________________________ Move to first element one level up ___
            o2jse.tv.f(targetObj.parentNode, upEl);
            }
        }
    // ********************************************************************** KeyRight ***
    else if (stdEvent.keyCode == KEY_RIGHT) {
        // ___________________ OPEN CHILD TREE OR MOVE TO FIRST LEVEL DOWN CURRENT ONE ___
        cL      = targetObj.parentNode.getElementsByTagName("span").length;
        nL      = (targetObj.parentNode.nextSibling ?
                   targetObj.parentNode.nextSibling.getElementsByTagName("span").length :
                   0);
        openBtn = targetObj.previousSibling.previousSibling;
        // ________________________________________________________ Open current level ___
        if (openBtn.className == "jx_treenode_close") {
            // ____________________________________ Override standard focus management ___
            var tvFocus = o2jse.createInput(o2jse.infoForm,
                                            false,
                                            false,
                                            targetObj.o2.c,
                                            "jxtvlastctrl");
            openBtn.click();
            o2jse.removeEl(tvFocus);
            }
        // __________________________________ Move on same level or next level is open ___
        else if ((cL == nL) ||
                ((cL < nL) && (openBtn.className == "jx_treenode_open"))) {
            // ________________________________ Move to first element down current one ___
            o2jse.tv.f(targetObj.parentNode, targetObj.parentNode.nextSibling);
            }
        }
    // ************************************************************************ PageUp ***
    else if (stdEvent.keyCode == KEY_PGUP) {
        // _____________________________________ SELECT FIRST ELEMENT OF CURRENT LEVEL ___
        var curEl = targetObj.parentNode;
        var upEl  = curEl;
        var cL    = curEl.getElementsByTagName("span").length;
        var pL    = cL;
        while (pL && pL == cL) {
            curEl = upEl;
            upEl  = upEl.previousSibling;
            pL    = upEl.getElementsByTagName("span").length;
            }
        // ____________________________________ Move to first element in current level ___
        o2jse.tv.f(targetObj.parentNode, curEl);
        }
    // ********************************************************************** PageDown ***
    else if (stdEvent.keyCode == KEY_PGDN) {
        // ______________________________________ SELECT LAST ELEMENT OF CURRENT LEVEL ___
        var curEl  = targetObj.parentNode;
        var nextEl = curEl;
        var cL     = curEl.getElementsByTagName("span").length;
        var nL     = cL;
        while (nL && nL == cL) {
            curEl  = nextEl;
            nextEl = nextEl.nextSibling;
            nL     = nextEl.getElementsByTagName("span").length;
            }
        // ____________________________________ Move to first element in current level ___
        o2jse.tv.f(targetObj.parentNode, curEl);
        }
    // ************************************************************************* ENTER ***
    else if (stdEvent.keyCode == KEY_ENTER) {
        openBtn = targetObj.previousSibling.previousSibling.className;
        // _________________________________________________ SELECT ITEM IF SELECTABLE ___
        if (targetObj.o2.aL == 1 ||
            (targetObj.o2.aL == 2 && openBtn == "jx_treenode_leaf") ||
            (targetObj.o2.aL == 3 && openBtn != "jx_treenode_leaf")) {
            targetObj.click();
            }
        }
    return false;

    };


/**
 * Move focus from a treeview element to another visible element
 *
 * @param {Object} elementFrom   Element moving focus from
 * @param {Object} elementTo     Element moving focus to
 */
o2jse.tv.f = function(elementFrom, elementTo) {

    if (!elementTo) {
        return false;
        }
    if (elementFrom.className == "jx_treenode_focus") {
        elementFrom.className = "";
        }
    if (elementTo.className != "jx_treenode_sele") {
        elementTo.className = "jx_treenode_focus";
        }
    // ___________________________________________________________ Text in source item ___
    if (elementFrom) {
        divFrom = elementFrom.lastElementChild;
        divFrom.removeAttribute("tabindex");
        delete divFrom.onkeydown;
        }
    // ___________________________________________________________ Text in target item ___
    divTo           = elementTo.lastElementChild;
    divTo.tabIndex  = 0;
    divTo.onkeydown = o2jse.tv.k;
    divTo.focus();
    return false;

    };


/**
 * Create context menu for the treeview control
 *
 */
o2jse.tv.initContMenu = function() {

    var jxInfo = o2jse.cMenu.target.o2;
    if (jxInfo.cT == "tree") {
        o2jse.tv.customInfo = jxInfo;
        if (o2jse.menu.menuList["jxTreeExtra"]) {
            o2jse.menu.menuList["jxTreeExtra"].clear();
            }
        o2jse.cMenu.addItem("M",
                            "jxTreeExtra",
                            "Treeview",
                            "",
                            o2jse.rntAlias + "img/tree/tree.png");
        // __________________________________________________________________ Fold all ___
        o2jse.menu.menuList["jxTreeExtra"].addItem("J", "jxTreeFold",
                                                   "Fold all",
                                                   o2jse.tv.foldAll,
                                                   o2jse.rntAlias + "img/tree/fold.png");
        // ________________________________________________________________ Unfold all ___
        o2jse.menu.menuList["jxTreeExtra"].addItem("J", "jxTreeUnfold",
                                                   "Unfold all",
                                                   o2jse.tv.unfoldAll,
                                                   o2jse.rntAlias +"img/tree/unfold.png");
        // _________________________________________________________________ Separator ___
        o2jse.menu.menuList["jxTreeExtra"].addItem("S");
/*
        // ____________________________________________________________ Filter records ___
        o2jse.menu.menuList["jxTreeExtra"].addItem("J", "jxTreeFilter",
                                                   "Filter elements",
                                                   o2jse.tv.filterElements,
                                                   o2jse.rntAlias +"img/tree/filter.png");
        // ______________________________________________________________ Sort records ___
        sortImg = "img/tree/" + (jxInfo.sorted ? "un" : "") + "sort.png";
        sortTxt = (jxInfo.sorted ? "Unsort" : "Sort") + " elements";
        o2jse.menu.menuList["jxTreeExtra"].addItem("J", "jxTreeSort",
                                                   sortTxt,
                                                   o2jse.tv.sortSwitch,
                                                   o2jse.rntAlias + sortImg);
*/
        // _______________________________________________________________ Export data ___
        o2jse.menu.menuList["jxTreeExtra"].addItem("J", "jxTreeExport",
                                                   "Export structure",
                                                   o2jse.tv.exportStructure,
                                                   o2jse.rntAlias +"img/tree/export.png");
/*
        // _________________________________________________________________ Separator ___
        o2jse.menu.menuList["jxTreeExtra"].addItem("S");
        // _____________________________________________________________________ Reset ___
        o2jse.menu.menuList["jxTreeExtra"].addItem("J", "jxTreeReset",
                                                   "Reset",
                                                   o2jse.tv.customReset,
                                                   o2jse.rntAlias +"img/tree/reset.png");
*/
        return true;
        }
    else {
        return false;
        }

    };


/**
 * Method executed on "Fold all" item from treeview context menu
 *
 */
o2jse.tv.foldAll = function() {

    o2jse.menu.closeAll();
    var jxInfo   = o2jse.tv.customInfo;
    var treeCtrl = o2jse.createInput(o2jse.infoForm,
                                     "hidden",
                                     "",
                                     jxInfo.c,
                                     "jxtree");
    var foldAll  = o2jse.createInput(o2jse.infoForm,
                                     "hidden",
                                     "",
                                     "1",
                                     "jxtreefoldall");
    if (o2jse.cliMode) {
        jxjs.request();
        }
    else {
        o2jse.cmd.submit();
        }
    o2jse.removeEl(treeCtrl);
    o2jse.removeEl(foldAll);

    };


/**
 * Method executed on "Unfold all" item from treeview context menu
 *
 */
o2jse.tv.unfoldAll = function() {

    o2jse.menu.closeAll();
    var jxInfo    = o2jse.tv.customInfo;
    var treeCtrl  = o2jse.createInput(o2jse.infoForm,
                                      "hidden",
                                      "",
                                      jxInfo.c,
                                      "jxtree");
    var unfoldAll = o2jse.createInput(o2jse.infoForm,
                                      "hidden",
                                      "",
                                      "1",
                                      "jxtreeunfoldall");
    if (o2jse.cliMode) {
        jxjs.request();
        }
    else {
        o2jse.cmd.submit();
        }
    o2jse.removeEl(treeCtrl);
    o2jse.removeEl(unfoldAll);

    };


/**
 * Method executed on "Unfold all" item from treeview context menu
 *
 */
o2jse.tv.exportStructure = function() {

    o2jse.menu.closeAll();
    var jxInfo     = o2jse.tv.customInfo;
    var treeCtrl   = o2jse.createInput(o2jse.infoForm,
                                       "hidden",
                                       "",
                                       jxInfo.c,
                                       "jxtree");
    var exportCtrl = o2jse.createInput(o2jse.infoForm,
                                       "hidden",
                                       "",
                                       "1",
                                       "jxtreeexport");
    if (o2jse.cliMode) {
        jxjs.request();
        }
    else {
        o2jse.cmd.submit();
        }
    o2jse.removeEl(treeCtrl);
    o2jse.removeEl(exportCtrl);

    };


/**
 * Method executed on "Sort/Unsort" item from treeview context menu
 *
 */
o2jse.tv.sortSwitch = function() {

    o2jse.menu.closeAll();
    var jxInfo   = o2jse.tv.customInfo;
    var treeCtrl = o2jse.createInput(o2jse.infoForm,
                                     "hidden",
                                     "",
                                     jxInfo.c,
                                     "jxtree");
    var sortCtrl = o2jse.createInput(o2jse.infoForm,
                                     "hidden",
                                     "",
                                     "1",
                                     "jxtreesort");
    if (o2jse.cliMode) {
        jxjs.request();
        }
    else {
        o2jse.cmd.submit();
        }
    o2jse.removeEl(treeCtrl);
    o2jse.removeEl(sortCtrl);

    };


/**
 * Images lister related functions collection
 *
 */
o2jse.il = {};


/**
 * Manage click on item events in images lister control
 *
 * @param object imglistObj   Images lister control
 * @param string item         Item unique ID
 */
o2jse.il.c = function(imglistObj, item) {

    o2jse.ctrl.init(imglistObj);
    var o2Info = imglistObj.o2;
    // ______________________________________________________________ Standard control ___
    if (o2Info.std) {
        o2jse.infoForm['o2lastform'].value = o2Info.f;
        o2jse.infoForm['o2lastctrl'].value = o2Info.c;
        var stdCtrl                        = o2jse.createInput(o2jse.infoForm,
                                                               false,
                                                               false,
                                                               item,
                                                               o2Info.c + o2Info.e);
        stdCtrl.o2                         = o2Info;
        if (o2jse.cliMode) {
            jxjs.request(stdCtrl, item);
            o2jse.removeEl(stdCtrl);
            }
        }
    // ___________________________________________ Scripting defined control (old way) ___
    else {
        var listerCtrl = o2jse.createInput(o2jse.infoForm,
                                           "hidden",
                                           "",
                                           o2Info.c,
                                           "jximglist");
        var itemCtrl   = o2jse.createInput(o2jse.infoForm,
                                           "hidden",
                                           "",
                                           item,
                                           "jximglistitem");
        if (o2jse.cliMode) {
            jxjs.request();
            o2jse.removeEl(listerCtrl);
            o2jse.removeEl(itemCtrl);
            }
        }
    if (!o2jse.cliMode) {
        o2jse.cmd.submit();
        }

    };


/**
 * Manage click on delete button events in images lister control
 *
 * @param object imglistObj   Images lister control
 * @param string item         Item unique ID
 */
o2jse.il.d = function(imglistObj, item) {

    o2jse.ctrl.init(imglistObj);
    var o2Info = imglistObj.o2;
    // ______________________________________________________________ Standard control ___
    if (o2Info.std) {
        o2jse.infoForm['o2lastform'].value = o2Info.f;
        o2jse.infoForm['o2lastctrl'].value = o2Info.c;
        var stdCtrl                        = o2jse.createInput(o2jse.infoForm,
                                                               false,
                                                               false,
                                                               item,
                                                               o2Info.c + o2Info.e);
        var delCtrl                        = o2jse.createInput(o2jse.infoForm,
                                                               false,
                                                               false,
                                                               '1',
                                                               o2Info.c + o2Info.e +
                                                               '_delete');
        stdCtrl.o2                         = o2Info;
        if (o2jse.cliMode) {
            jxjs.request(stdCtrl, item);
            o2jse.removeEl(stdCtrl);
            o2jse.removeEl(delCtrl);
            }
        }
    // ___________________________________________ Scripting defined control (old way) ___
    else {
        var listerCtrl = o2jse.createInput(o2jse.infoForm,
                                           "hidden",
                                           "",
                                           ctrl,
                                           "jximglist");
        var itemCtrl   = o2jse.createInput(o2jse.infoForm,
                                           "hidden",
                                           "",
                                           item,
                                           "jximglistdelete");
        if (o2jse.cliMode) {
            jxjs.request();
            o2jse.removeEl(listerCtrl);
            o2jse.removeEl(itemCtrl);
            }
        }
    if (!o2jse.cliMode) {
        o2jse.cmd.submit();
        }

    };


/**
 * Fast Message Box related functions collection
 *
 */
o2jse.fastMsg = {

    frameObj : null, /* Frame to collect multiple messages in                           */
    msgWin   : [],   /* Notification window object (<DIV>) list                         */
    closeBtn : []    /* Close buttons list (one for each msgWin)                        */

    };


/**
 * Returns frame object containing all messages.
 * If it doesn't exist it is created first time.
 *
 */
o2jse.fastMsg.frame = function() {

    if (!o2jse.fastMsg.frameObj) {
        // ______________________________________________________________ Frame object ___
        o2jse.fastMsg.frameObj = o2jse.createEl(document.body, "div", "jxfastmsgframe");
        o2jse.fastMsg.frameObj.style.position = "absolute";
        }
    return o2jse.fastMsg.frameObj;

    };


/**
 * Show a message in the Fast Message window
 *
 * @param {String} msgCode   HTML code for message to display
 */
o2jse.fastMsg.show = function(msgCode) {

    // ___________________________________________________ Assign unique ID to message ___
    var winId = 0;
    do {
        winId = (new Date).getMilliseconds();
       } while (winId in o2jse.fastMsg.msgWin);
    // _______________________________________________________________________ Out box ___
    var msgDiv = o2jse.createEl(o2jse.fastMsg.frame(), 'div', 'jxfastmsg');
    msgDiv.innerHTML              = msgCode;
    // __________________________________________________________________ Close button ___
    var closeBtn                  = o2jse.createEl(msgDiv, 'div', 'jxfastmsg_close',
                                                   '<img src="' +
                                                    o2jse.rntAlias + 'img/close.png">');
    msgDiv.style.position         = "relative";
    msgDiv.onmouseover            = function() {
                                       closeBtn.style.display = 'block';
                                       o2jse.fastMsg.unhide(winId);
                                       };
    msgDiv.onmouseout             = function() {
                                       closeBtn.style.display = 'none';
                                       msgDiv.steps           = [];
                                   msgDiv.steps[0] = setTimeout("o2jse.fastMsg.fadeOut(" +
                                                                winId + ")",
                                                                o2jse.fastMsgTime * 1000);
                                        };
    // ____________________________________________________ List of activated timeouts ___
    msgDiv.steps                  = [];
    msgDiv.steps[0]               = setTimeout("o2jse.fastMsg.fadeOut(" + winId + ")",
                                               o2jse.fastMsgTime * 1000);
    closeBtn.style.display        = 'none';
    closeBtn.onclick              = function() { o2jse.fastMsg.hide(winId); };
    o2jse.fastMsg.msgWin[winId]   = msgDiv;
    o2jse.fastMsg.closeBtn[winId] = closeBtn;

    };


/**
 * Hide the Fast Message window
 *
 * @param {Integer} winId
 */
o2jse.fastMsg.hide = function(winId) {

    for (var i = o2jse.fastMsg.msgWin[winId].steps.length - 1; i >= 0; i--) {
        clearTimeout(o2jse.fastMsg.msgWin[winId].steps[i]);
        }
    o2jse.fastMsg.msgWin[winId].steps = [];
    if (o2jse.fastMsg.msgWin[winId]) {
        o2jse.removeEl(o2jse.fastMsg.msgWin[winId]);
        delete o2jse.fastMsg.msgWin[winId];
        o2jse.removeEl(o2jse.fastMsg.closeBtn[winId]);
        delete o2jse.fastMsg.closeBtn[winId];
        }

    };


/**
 * Stop hiding the Fast Message window
 *
 * @param {Integer} winId
 */
o2jse.fastMsg.unhide = function(winId) {

    for (var i = o2jse.fastMsg.msgWin[winId].steps.length - 1; i >= 0; i--) {
        clearTimeout(o2jse.fastMsg.msgWin[winId].steps[i]);
        }
    o2jse.fastMsg.msgWin[winId].steps = [];
    o2jse.fastMsg.setOpacity(winId, 100);

    };


/**
 * Set opacity level for the Fast Message box
 *
 * @param {Integer} winId
 * @param {Integer} opLevel
 */
o2jse.fastMsg.setOpacity = function(winId, opLevel) {

    if (o2jse.fastMsg.msgWin[winId]) {
        o2jse.cmd.opacity(o2jse.fastMsg.msgWin[winId], opLevel);
        }

    };


/**
 * Set opacity decreasing for message fading out
 *
 * @param {Integer} winId
 */
o2jse.fastMsg.fadeOut = function(winId) {

    var timer = 0;
    for (var i = 100; i >= 1; i--) {
        o2jse.fastMsg.msgWin[winId].steps[i] =
              setTimeout("o2jse.fastMsg.setOpacity(" + winId + ", " + i + ")", timer * 7);
        timer++;
        }
    o2jse.fastMsg.msgWin[winId].steps[o2jse.fastMsg.msgWin[winId].steps.length] =
                               setTimeout("o2jse.fastMsg.hide(" + winId + ")", timer * 7);

    };


/**
 * Notification area related functions collection
 *
 */
o2jse.notify = {

    notifyWin : null,  /* Notification window object (<DIV>)                            */
    winOpen   : false, /* If notification window is actualy displayed                   */
    itemsList : [],    /* Items list defined by response code                           */
    timeOut   : null   /* Timeout reference                                             */

    };


/**
 * Handler for click events on notification area
 *
 */
o2jse.notify.clickOnIcon = function() {

    if (o2jse.notify.winOpen) {
        o2jse.notify.hideWin();
        }
    else {
        o2jse.notify.exeReq();
        o2jse.ctrl.make_waiting(document.getElementById("jxnotify"));
        o2jse.notify.createWin();
        }

    };


/**
 * Creates and returns notification window
 *
 */
o2jse.notify.createWin = function() {

    var iList       = document.createElement("TABLE");
    iList.className = "o2notify_list";
    var empty       = true;
    // ____________________________________________________________ Read items context ___
    for (var l in o2jse.notify.itemsList) {
        item          = o2jse.notify.itemsList[l];
        iRow          = o2jse.createEl(iList,
                                       "TR",
                                       "o2notify_row" + (item[3] ? "_on" : "_off"));
        body          = o2jse.textDecode(item[1]);
        iRow.title    = (body.length > 100 ? body.substr(0, 94) + "[...]" : body);
        iRow.jxMsgID  = l;
        iRow.jxMsgAct = (item[3] ? true : false);
        iRow.onclick  = function() { o2jse.notify.clickOnDispatch(this); };
        // _____________________________________________________________________ Image ___
        iImgTd        = o2jse.createEl(iRow, "TD", "o2notify_img");
        iImg          = o2jse.createEl(iImgTd, "IMG");
        // _______________________________________________________________ Custom icon ___
        if (item[2]) {
            iImg.src = item[2];
            }
        else {
            iImg.src = o2jse.rntAlias + "img/notify/msg.png";
            }
        // _____________________________________________________________________ Title ___
        iTxt          = o2jse.createEl(iRow, "TD", "", item[0]);
        empty         = false
        }
    if (empty) {
        iList           = document.createElement("DIV");
        iList.innerHTML = "<div style='margin:10px;font-style:italic;'>No messages</div>";
        }
    // _______________________________________________________________________ Out box ___
    o2jse.notify.notifyWin = o2jse.createEl(document.body, "div", "o2notify_win");
    o2jse.notify.notifyWin.appendChild(iList);
    o2jse.notify.winOpen = true;

    };


/**
 * Hides notification window
 *
 */
o2jse.notify.hideWin = function() {

    if (o2jse.notify.winOpen) {
        if (o2jse.notify.notifyWin) {
            o2jse.removeEl(o2jse.notify.notifyWin);
            delete o2jse.notify.notifyWin;
            }
        o2jse.notify.winOpen = false;
        }

    };


/**
 * Executes AJAX-style request for lookup items list. Returned page is recovered by
 * callback method o2jse.notify.getList().
 *
 */
o2jse.notify.exeReq = function() {

    o2jse.requester.exe('notify',
                        'JXSESSNAME=' + o2jse.sessName,
                        o2jse.notify,
                        o2jse.notify.getList,
                        true);

    };


/**
 * Manage lookup list AJAX-style receiving. Page, requested by method
 * o2jse.ctrl.exeReq(), is recovered and analysed to get items lines.
 *
 * @param object nullObj    Parameter always passed as NULL
 * @param string listText   Returned page body text containing HTML rows
 */
o2jse.notify.getList = function(nullObj, listText) {

    // Update list, if window open (Add to code because code evaluation is asynchronous) _
    listText+= "if (o2jse.notify.winOpen) { " +
               "o2jse.notify.hideWin();" +
               "o2jse.notify.createWin(); }";
    o2jse.exeCode(listText);
    var unread = 0;
    for (var l in o2jse.notify.itemsList) {
        if (!o2jse.notify.itemsList[l][4]) {
            unread++;
            }
        }
    // ____________________________________________________________ Update status icon ___
    var notifyArea   = document.getElementById("jxnotify");
    // _____________________________________________________________ Remove wait-image ___
    if (o2jse.waitObj) {
        o2jse.removeEl(o2jse.waitObj);
        delete o2jse.waitObj;
        }
    notifyArea.style.display = "block";
    notifyArea.title         = (unread ?
                                "You have " + unread + " unread messages" :
                                "No messages");
    // _______________________________________________________________ Set notify icon ___
    notifyArea.getElementsByTagName("IMG")[0].src = o2jse.rntAlias + "img/notify/" +
                                                    (unread ? "msg.png" : "empty.png");
    notifyArea.getElementsByTagName("DIV")[0].innerHTML = (unread ? unread : "");
    // _________________________________________________________ Reset refresh timeout ___
    if (o2jse.notify.timeOut) {
        clearTimeout(o2jse.notify.timeOut);
        o2jse.notify.timeOut = null;
        }
    o2jse.notify.timeOut   = setTimeout(o2jse.notify.exeReq, o2jse.refreshTime);
    o2jse.notify.inRequest = false;

    };


/**
 * Handler for click events on single dispatch notification: manage activable dispatches
 * activation
 *
 * @param object trObj
 */
o2jse.notify.clickOnDispatch = function(trObj) {

    if (trObj.jxMsgAct) {
        var fields           = [];
        fields['JXSESSNAME'] = o2jse.sessName;
        if (o2jse.infoForm[o2jse.sessName]) {
            fields[o2jse.sessName] = o2jse.infoForm[o2jse.sessName].value;
            }
        fields['jxact']   = 'dispatch';
        fields['jxmsgid'] = trObj.jxMsgID;
        o2jse.cmd.post(false, fields);
        }
    else {
        o2jse.removeEl(trObj);
        o2jse.requester.exe("remdispatch",
                            "jxmsgid=" + trObj.jxMsgID,
                            o2jse.notify,
                            null);
        // ________________________________________________________ Update status icon ___
        var notifyArea   = document.getElementById("jxnotify");
        var notifyText   = notifyArea.getElementsByTagName("DIV")[0];
        var unread       = parseInt(notifyText.innerHTML) - 1;
        notifyArea.title = (unread ?
                            "You have " + unread + " unread messages" :
                            "No messages");
        // ___________________________________________________________ Set notify icon ___
        notifyArea.getElementsByTagName("IMG")[0].src = o2jse.rntAlias + "img/notify/" +
                                                       (unread ? "msg.png" : "empty.png");
        notifyText.innerHTML                          = (unread ? unread : "");
        }

    };


/**
 * Progress bar related functions collection
 *
 */
o2jse.progress = {

    active : false,
    timeOut : null

    };


/**
 * Sets progress bar value
 *
 */
o2jse.progress.set = function(barID, barProgress) {

    var barEl = document.getElementById(barID);
    var barElEls;
    if (barEl) {
        barElEls                = barEl.getElementsByTagName("DIV");
        barElEls[0].style.width = barProgress + "%";
        barElEls[1].innerHTML   = barProgress + "%";
        }

    };


/**
 * Activates timeout to refresh active progress bars
 *
 */
o2jse.progress.start = function() {

    o2jse.progress.active = true;

    };


/**
 * Stops timeout refreshing active progress bars
 *
 */
o2jse.progress.stop = function() {

    o2jse.progress.active = false;

    };


/**
 * Manage progress-bar update AJAX-style receiving. Page, requested by method
 * o2jse.requester.exe(), is recovered and analysed to get new progress value.
 *
 * @param {Object} reqObj
 * @param {String} reqJs
 */
o2jse.progress.getCode = function(reqObj, reqJs) {

    // ________________________________________________________ Evaluate response code ___
    o2jse.exeCode(reqJs);
    if (o2jse.progress.active && !o2jse.progress.timeOut) {
        o2jse.progress.timeOut = setTimeout(function() {
                                                delete o2jse.progress.timeOut;
                                                o2jse.requester.exe('progress',
                                                                    'JXSESSNAME=' +
                                                                    o2jse.sessName,
                                                                    o2jse.progress,
                                                                   o2jse.progress.getCode,
                                                                    true);
                                                },
                                            3000);
        }


    };


/**
 * Javascript definition controls related functions collection
 *
 */
var jxjs = {

    reqId       : 0,    /* Request unique ID to match against response                  */
    resId       : 0,    /* Response unique ID created by request                        */
    scriptsList : [],   /* List of scripts carried by windows HTML code                 */
    waitingCtrl : null, /* Control to be restored displaying waiting image              */
    dbg_ctrl    : null, /* Evaluating control trace for debug                           */
    respTimeOut : null, /* Time-out for response to return                              */
    cachedCmd   : '',   /* Commands cached by sub-on-change to be executed on response  */
    cachedObj   : null, /* Target cached by sub-on-change to be processed on response   */
    cachedEvent : null, /* Cached event by sub-on-change to be processed on response    */
    extPars     : []    /* List of fields used as extra-parameters by actions           */

    };


/**
 * Manage requests to the HTTP-Requester instead of submitting pages when the cli-mode is
 * active.
 *
 * @param {Object} reqObj
 * @param {Mix}    reqValue
 * @param {Mix}    refrAct
 */
jxjs.request = function(reqObj, reqValue, refrAct) {

    if (reqObj) {
        o2jse.ctrl.init(reqObj);
        var o2Info = reqObj.o2;
        o2jse.infoForm['o2_prgexeid'].value = o2Info.e;
        if (o2Info.cT != "button") {
            if (o2jse.infoForm['o2_modfields'].value.indexOf(o2Info.c + ";") < 0) {
                o2jse.infoForm['o2_modfields'].value += o2Info.c + ";";
                }
            o2jse.infoForm[o2Info.c + o2Info.e].value = reqValue;
            }
        }
    var lcsW = window.innerWidth;
    var lcsH = window.innerHeight;
    // ______________________________________________________________________ Menu Bar ___
    if (o2jse.menu.appMainMenu) {
        // _____________________________________________ Menu Bar TOP - Vertical space ___
        if (o2jse.menuStyle == 'T') {
            var lmbH = document.getElementById('jxMenuBar').offsetHeight;
            if (parseInt(lmbH) > 0) {
                o2jse.infoForm['jxmbh'].value = lmbH;
                }
            }
        // __________________________________________ Menu Bar LEFT - Horizontal space ___
        else {
            var lmbW = document.getElementById('jxMenuBar').offsetWidth;
            if (parseInt(lmbW) > 0) {
                o2jse.infoForm['jxmbw'].value = lmbW;
                }
            }
        }
    // ___________________________________________________ Status Bar - vertical space ___
    if (statusBar = document.getElementById("o2status")) {
        o2jse.infoForm['jxsbh'].value = statusBar.offsetHeight;
        }
    if (parseInt(lcsW) > 0) {
        o2jse.infoForm['jxcsw'].value = lcsW;
        }
    if (parseInt(lcsH) > 0) {
        o2jse.infoForm['jxcsh'].value = lcsH;
        }
    // ________________________________ In case of file upload use standard submit _______
    if (o2jse.ctrl.upLoad) {
        return o2jse.cmd.submit(reqObj ? o2Info.e : 0);
        }
    this.reqId = new Date().getTime();
    if (!jxjs.respTimeOut) {
        jxjs.respTimeOut = setTimeout(jxjs.onLateResp, (o2jse.maxMultiReq * 500));
        }
    o2jse.submitting = true;
    if (refrAct) {
        o2jse.infoForm['o2_action'].value = (refrAct == true ? "" : refrAct);
        o2jse.requester.exe('refresh', 'jxjsid=' + this.reqId, jxjs, jxjs.jsEval);
        }
    else {
        o2jse.requester.exe('pagepost', 'jxjsid=' + this.reqId, jxjs, jxjs.jsEval);
        }
    o2jse.infoForm['o2_modfields'].value = "";
    o2jse.infoForm['o2_action'].value    = "";
    o2jse.infoForm['o2lastctrl'].value   = "";
    o2jse.infoForm['o2lastform'].value   = "";
    o2jse.infoForm['win_list'].value     = "";
    o2jse.infoForm['win_pos'].value      = "";
    o2jse.infoForm['win_size'].value     = "";
    // ____________________________________ Free extra parameters fields for next uses ___
    if (o2jse.extPars) {
        for (var i = o2jse.extPars.length - 1; i >= 0; i--) {
            o2jse.removeEl(o2jse.extPars[i]);
            }
        o2jse.extPars = [];
        }

    };


/**
 * Refresh page context in AJAX mode, after execution of action refrAct (if any)
 *
 * @param {Array}  formInfo
 * @param {String} refrAct
 */
jxjs.refresh = function(formInfo, refrAct) {

    if (formInfo) {
        if (o2jse.infoForm['o2lastform'].value != formInfo.f) {
            o2jse.infoForm['o2lastctrl'].value = '';
            }
        o2jse.infoForm['o2lastform'].value = formInfo.f;
        }
    if (!refrAct) {
        refrAct = true;
        }
    jxjs.request(null, null, refrAct);

    };


/**
 * Callback function activated by jxjs.respTimeOut on late responses.
 *
 */
jxjs.onLateResp = function() {

    o2jse.cmd.showClock(true);

    };


/**
 * Manage requester end on FULL-AJAX mode. Evaluate returned script to update interface.
 *
 * @param {Object} reqObj
 * @param {String} reqJs
 */
jxjs.jsEval = function(reqObj, reqJs) {

    // _________________________________________________________ Reset waiting control ___
    if (o2jse.waitObj) {
        o2jse.removeEl(o2jse.waitObj);
        delete o2jse.waitObj;
        }
    if (jxjs.waitingCtrl) {
        jxjs.waitingCtrl.style.display = "block";
        }
    // ________________________________________________________ Evaluate response code ___
    o2jse.started = false;
    o2jse.exeCode(reqJs + jxjs.cachedCmd);
    jxjs.cachedCmd   = '';
    jxjs.cachedObj   = null;
    jxjs.cachedEvent = null;
    // _________________________________________________ Execute HTML embedded scripts ___
    if (jxjs.scriptsList.length) {
        var scriptList = "";
        for (scriptID in jxjs.scriptsList) {
            if (jxjs.scriptsList[scriptID].trim() != "") {
                scriptList+= jxjs.scriptsList[scriptID] + "\n";
                }
            }
        jxjs.scriptsList = [];
        jxjs.dbg_ctrl    = "Embedded script";
        o2jse.exeCode(scriptList);
        }
    o2jse.init();

    };


/**
 * Manage requester end on FULL-AJAX mode. Evaluate returned script to update interface.
 *
 * @param {String} jxact
 * @param {Object} params
 */
jxjs.beacon = function(jxact, params) {

    var fd = new FormData();
    fd.append('JXSESSNAME', o2jse.sessName);
    fd.append('jxact', jxact);
    if (params && Object.keys(params).length) {
        Object.keys(params).forEach(function(k) { fd.append(k, params[k]); });
        }
    navigator.sendBeacon(o2jse.rntAlias + 'jxr.php', fd);
    delete(fd);

    };


/**
 * Evaluates Javascript code in HTML returned by response in FULL-AJAX mode
 *
 * @param {Object} container   HTML element where code is contained
 */
jxjs.runScripts = function(container) {

    var codes = container.getElementsByTagName("SCRIPT");
    if (codes.length) {
        for (var codeID = codes.length - 1; codeID >= 0; codeID--) {
            jxjs.scriptsList[jxjs.scriptsList.length] = codes[codeID].innerHTML;
            }
        }

    };


/**
 * Client-side error reporting in FULL-AJAX mode
 *
 * @param {String} errCode
 */
jxjs.cError = function(ex, reqJs){

    if (jxjs.respTimeOut) {
        clearTimeout(jxjs.respTimeOut);
        jxjs.respTimeOut = null;
        }
    if (o2jse.devName) {
        jsErr = "<div id='jxjsErr' style='position:relative;z-index:9998;'>\n" +
                "<table class='o2_error_report'>\n" +
                "<tr><td COLSPAN='2'><table>\n" +
                "<tr>\n<td class='o2_error_title'><center>" +
                "<b>AJAX REQUEST FAILURE</b></center></td>\n" +
                "<td class='o2_error_close' onclick='jxjsErr.style.display=\"none\";'" +
                " title='Close error message'>X</td></tr></table>\n</tr>\n" +
                "<tr><td COLSPAN='2'class='o2_error_text'>" + ex.name +
                "</td></tr>\n<tr><td class='o2_error_info'><i>Description:</i></td>" +
                "<td class='o2_error_info'>" + ex.message +
                "</td></tr>\n<tr><td class='o2_error_info'><i>Line:</i></td>" +
                "<td class='o2_error_info'>" + (ex.lineNumber + 1) +
                "</td></tr>\n<tr><td class='o2_error_info'><i>Request state:</i>" +
                "</td><td class='o2_error_info'>" + jxjs.dbg_ctrl +
                "</td></tr>\n<tr>\n<td COLSPAN='2' class='o2_error_info'><pre>" +
                reqJs + "</pre></td>\n</tr>\n</table></div>\n";
        document.body.innerHTML = jsErr + document.body.innerHTML;
        }
    document.getElementById("o2statustext").innerHTML = "<b>Error in response</b>";
    o2jse.submitting                                  = false;
    o2jse.cliMode                                     = false;

    };


/**
 * Check response ID against requests progressive ID.
 * Response script is evaluated only when matching IDs, to avoid obsolete scripts
 * execution.
 *
 * @param {Integer} resId
 */
jxi = function(resId) {

    var yorn = (resId > jxjs.resId);
    if (yorn) {
        if (jxjs.respTimeOut) {
            clearTimeout(jxjs.respTimeOut);
            jxjs.respTimeOut = null;
            }
        jxjs.resId = resId;
        }
    else {
        o2jse.infoForm['o2_modfields'].value = '';
        o2jse.infoForm['o2_action'].value    = '';
        o2jse.infoForm['o2lastform'].value   = '';
        o2jse.infoForm['o2lastctrl'].value   = '';
        o2jse.reposWins                      = [];
        jxjs.scriptsList                     = [];
        o2jse.cmd.submit();
        }
    return yorn;

    };


/**
 * Requester update manager for controls in FULL-AJAX mode
 *
 * @param {Object} defObj
 */
jxc = function(defObj) {

    var ctrlObj;
    var modFields = o2jse.infoForm["o2_modfields"];
    jxjs.dbg_ctrl = "Processing " + defObj.i + " (" + defObj.t + ")";
    // ======================================================================== WINDOW ===
    if (defObj.t == "win") {
        // _________________________________________________________ Window is visible ___
        if (defObj.v) {
            // ________________________________________________ If window code is sent ___
            if (defObj.code) {
                // _____________________________ Window passed from active to inactive ___
                if (ctrlObj = document.getElementById(defObj.i)) {
                    ctrlObj.outerHTML = defObj.code;
                    }
                // ____________________________________________ Window is a new window ___
                else {
                    contDiv = o2jse.createEl(document.getElementById("jxjsinsert"),
                                             "div");
                    // _____________ InnerHTML is set after to avoid SCRIPTs execution ___
                    contDiv.innerHTML = defObj.code;
                    contDiv.jxDynamic = true;
                    jxjs.runScripts(contDiv);
                    }
                }
            if (defObj.title && (ctrlObj = document.getElementById(defObj.i + "_title"))){
                if (ctrlObj.innerHTML != defObj.title) {
                    ctrlObj.innerHTML = defObj.title;
                    }
                }
            if ((typeof defObj.x != 'undefined') || (typeof defObj.y != 'undefined')) {
                o2jse.win.needRepos(defObj.i);
                // _______________________________________________________ X-Y element ___
                ctrlObj = document.getElementById(defObj.i);
                o2jse.ctrl.init(ctrlObj);
                // ___________________________________ Change position by absolute X-Y ___
                ctrlObj.o2.alignH = "";
                ctrlObj.o2.alignV = "";
                ctrlObj.o2.x      = defObj.x;
                ctrlObj.o2.y      = defObj.y;
                }
            // _________________________________________________________ Width element ___
            ctrlObj = document.getElementById(defObj.i).getElementsByTagName("div")[0];
            // __________________________________________________________ Change width ___
            if (parseInt(ctrlObj.offsetWidth) != defObj.w) {
                ctrlObj.style.width = defObj.w + "px";
                }
            // ________________________________________________________ Height element ___
            ctrlObj = document.getElementById(defObj.i + "_in");
            // _________________________________________________________ Change height ___
            if (parseInt(ctrlObj.offsetHeight) != defObj.h) {
                ctrlObj.style.height = defObj.h + "px";
                }
            }
        else {
            // ________________________________________________ Remove existing window ___
            var winDiv = document.getElementById(defObj.i);
            if (winDiv) {
                if (winDiv.parentNode.jxDynamic) {
                    o2jse.removeEl(winDiv.parentNode);
                    }
                else {
                    o2jse.removeEl(winDiv);
                    }
                o2jse.win.needNoRepos(defObj.i);
                }
            }
        jxjs.dbg_ctrl = "Processed " + defObj.i + " (win)";
        }
    // ===================================================================== MULTIPAGE ===
    else if (defObj.t == "multipage") {
        // ________________________________________________________________ Get object ___
        if (ctrlObj = document.getElementById(defObj.i + "_out")) {
            // _____________________________________________ Set visibility and styles ___
            if (defObj.v) {
                ctrlObj.style.display         = "";
                ctrlObj.style.width           = defObj.w + 'px';
                ctrlObj.style.height          = defObj.h + 'px';
                ctrlObj.parentNode.style.left = defObj.x + 'px';
                ctrlObj.parentNode.style.top  = defObj.y + 'px';
                // ____ Selection is forced when multipage value is set programmatelly ___
                if (defObj.select) {
                    o2jse.infoForm[defObj.i].value = defObj.d;
                    // _______________________________________ Loop on multipage pages ___
                    var mpPages = ctrlObj.childNodes;
                    for (var pageNum = mpPages.length - 1; pageNum >= 0; pageNum--) {
                        // _________________________________________ Set buttons style ___
                        var mpBtn;
                        if (mpBtn = document.
                                     getElementById(defObj.i + "_mpb" + pageNum)) {
                            o2jse.ctrl.init(mpBtn);
                            // _____________________________________________ Button ON ___
                            if (pageNum == defObj.d) {
                                mpBtn.className = defObj.cssOn;
                                mpBtn.tabIndex  = "-1";
                                mpBtn.onclick   = null;
                                }
                            // _________________________________________ Button(s) OFF ___
                            else {
                                mpBtn.className = defObj.cssOff;
                                mpBtn.tabIndex  = "0";
                                mpBtn.onclick   = function() {
                                                     o2jse.ctrl.multiPage(this);
                                                     };
                                }
                            }
                        // ______________________________________ Set pages visibility ___
                        var mpPage = mpPages[pageNum];
                        if (mpPage.tagName && mpPage.tagName.toLowerCase() == "div") {
                            // ________________ Set active page content and visibility ___
                            if (mpPage.id == defObj.i + "_p" + defObj.d) {
                                mpPage.style.display = "";
                                if (defObj.code) {
                                    mpPage.innerHTML = defObj.code;
                                    jxjs.runScripts(mpPage);
                                    }
                                }
                            // _________________________ Set all other pages invisible ___
                            else if (mpPage.id.substr(0, defObj.i.length + 2) ==
                                     defObj.i + "_p") {
                                mpPage.style.display = "none";
                                }
                            }
                        }
                    }
                // _____________________ Multipage changed by clicking on control tabs ___
                else if (defObj.code) {
                    // _______________________________________ Set active page content ___
                    var objDivOpen = document.getElementById(defObj.i + "_p" + defObj.d);
                    objDivOpen.innerHTML = defObj.code;
                    jxjs.runScripts(objDivOpen);
                    }
                }
            else {
                ctrlObj.style.display = "none";
                }
            jxjs.dbg_ctrl = "Processed " + defObj.i + " (multipage)";
            }
        else {
            jxjs.dbg_ctrl = "Missing " + defObj.i + " (multipage)";
            }
        }
    // ========================================================================= TABLE ===
    else if (defObj.t == "tab") {
        // ________________________________________________________________ Get object ___
        var tabObj  = document.getElementById(defObj.i);
        var tabCont = tabObj.parentNode;
        // ____________________________________________________________ Set visibility ___
        if (defObj.v) {
            tabCont.style.display = "";
            tabObj.style.width    = defObj.w + 'px';
            tabObj.style.height   = defObj.h + 'px';
            tabCont.style.left    = defObj.x + 'px';
            tabCont.style.top     = defObj.y + 'px';
            // _______________________________________________ Table HTML content send ___
            if (defObj.code) {
                // _______________________________________________________ Set content ___
                if (tabCont.innerHTML != defObj.code) {
                    tabCont.innerHTML = defObj.code;
                    jxjs.runScripts(tabCont);
                    }
                }
            // ________________________________________________ Last modified row sent ___
            else if (defObj.rowList) {
                var tRows   = document.getElementById(defObj.i + "_tab").
                               getElementsByTagName("tbody")[0].rows;
                var rowNum  = defObj.lastRow;
                var lastRow = tRows[rowNum];
                if (defObj.foot) {
                    var myFoot = document.getElementById(defObj.i + '_pseudoF').
                                  getElementsByTagName('tfoot')[0].
                                   getElementsByTagName('tr');
                    }
                for (var cellId in defObj.rowList) {
                    var valObj  = null;
                    var newCell = defObj.rowList[cellId];
                    var oldCell = tRows[rowNum + newCell.r].cells[newCell.c];
                    // _____________________________________ Exclude invisible columns ___
                    if (typeof oldCell != "undefined") {
                        // ___________________ Value in DIV (edit, button, combo, ...) ___
                        if (valObj = oldCell.getElementsByTagName("div")[0]) {
                            valObj.innerHTML = (newCell.v != "" ? newCell.v : "&nbsp;");
                            // _______________________________ Set control style class ___
                            if (valObj.className && valObj.className != newCell.t) {
                                valObj.className = newCell.t;
                                }
                            }
                        // __________________________________________________ Checkbox ___
                        else if (valObj = oldCell.getElementsByTagName("input")[0]) {
                            valObj.checked = (newCell.v == "1");
                            valObj.value   = newCell.v;
                            // _______________________________ Set control style class ___
                            if (valObj.className && valObj.className != newCell.t) {
                                valObj.className = newCell.t;
                                }
                            }
                        // _____________________________________________________ Image ___
                        else if (valObj = oldCell.getElementsByTagName("img")[0]) {
                            valObj.src = newCell.v;
                            }
                        // ______________________________________ Set cell style class ___
                        if (oldCell.className != newCell.s) {
                            oldCell.className = newCell.s;
                            }
                        var tdObj = valObj.parentNode;
                        tdObj.onmouseover = null;
                        tdObj.onmouseout  = null;
                        }
                    }
                // _______________________________________________________ Footer sent ___
                if (defObj.foot) {
                    for (var footC in defObj.foot) {
                        myFootC = defObj.foot[footC];
                        myFoot[myFootC.r].getElementsByTagName('td')[myFootC.c].
                         innerText = myFootC.l;
                        }
                    }
                // _____________________________________________ Set row o2 properties ___
                lastRow.o2 = defObj.rowprop;
                // _______________________________________________ Set row style class ___
                if (lastRow.className != defObj.rowprop.cssc) {
                    lastRow.className = defObj.rowprop.cssc;
                    }
                }
            }
        else {
            tabCont.style.display = "none";
            }
        }
    // ====================================================================== DOCUMENT ===
    else if (defObj.t == "doc") {
        // ________________________________________________________________ Get object ___
        var docFrame = document.getElementById(defObj.i);
        // ____________________________________________________________ Set visibility ___
        if (defObj.v) {
            docFrame.style.display         = "";
            docFrame.style.width           = defObj.w + 'px';
            docFrame.style.height          = defObj.h + 'px';
            docFrame.parentNode.style.left = defObj.x + 'px';
            docFrame.parentNode.style.top  = defObj.y + 'px';
            // _______________________________________________________________ Set URL ___
            if (defObj.u) {
                docFrame.src = defObj.u;
                }
            }
        else {
            docFrame.style.display = "none";
            }
        }
    // ============================================= Get object for all other controls ===
    else {
        // ______________________________________________ Object is element of a table ___
        if (defObj.r && defObj.t != "listcombo" && o2jse.infoForm[defObj.i]) {
            // ___________________________________ Object is inside an array of fields ___
            if (!(ctrlObj = o2jse.infoForm[defObj.i][defObj.r - 1])) {
                ctrlObj = o2jse.infoForm[defObj.i];
                }
            }
        // _______________________________________ Try to get element by name or by ID ___
        else if (!(ctrlObj = o2jse.infoForm[defObj.i] ||
                             document.getElementById(defObj.i))) {
            jxjs.dbg_ctrl = "Missing " + defObj.i + " (" + defObj.t + ")";
            return;
            }
        // _________________________________________________________ Set o2 properties ___
        o2_save    = ctrlObj.o2;
        ctrlObj.o2 = defObj.p;
        switch (defObj.t) {
            // ================================================================= LABEL ===
            case "label":
                // ____________________________________________________ Set visibility ___
                if (defObj.v) {
                    ctrlObj.style.display = "";
                    if (defObj.p.pT != 'tab') {
                        ctrlObj.parentNode.style.left = defObj.x + 'px';
                        ctrlObj.parentNode.style.top  = defObj.y + 'px';
                        ctrlObj.style.width           = defObj.w + 'px';
                        }
                    // _______________________ Edit, text and combo in disabled tables ___
                    else if (o2_save.cT != 'label') {
                        ctrlObj.style.maxWidth = defObj.w + 'px';
                        }
                    else {
                        ctrlObj.style.width = defObj.w + 'px';
                        }
                    ctrlObj.style.height = defObj.h + 'px';
                    // _______________________________________________ Set style class ___
                    if (ctrlObj.className != defObj.s) {
                        ctrlObj.className = defObj.s;
                        }
                    // _____________________________________________________ Set value ___
                    if (ctrlObj.innerHTML != defObj.l) {
                        ctrlObj.innerHTML = defObj.l;
                        }
                    }
                else {
                    ctrlObj.style.display = "none";
                    }
                break;
            // ================================================================== EDIT ===
            case "edit":
                // ____________________________________________________ Set visibility ___
                if (defObj.v) {
                    ctrlObj.style.display = "";
                    ctrlObj.style.width   = defObj.w + 'px';
                    ctrlObj.style.height  = defObj.h + 'px';
                    if (defObj.p.pT != 'tab') {
                        ctrlObj.parentNode.style.left = defObj.x + 'px';
                        ctrlObj.parentNode.style.top  = defObj.y + 'px';
                        }
                    // __________________________________________________ Set editable ___
                    var disabled = ctrlObj.nodeName != "INPUT";
                    if (disabled == defObj.e) {
                        // ____________________________________ Replace DIV with INPUT ___
                        if (defObj.e) {
                            var newEl          = document.createElement("input");
                            newEl.style.width  = ctrlObj.style.width;
                            newEl.style.height = ctrlObj.style.height;
                            newEl.name         = ctrlObj.id;
                            newEl.o2           = defObj.p;
                            newEl.onchange     = function() { o2jse.ctrl.c(this); };
                            newEl.onfocus      = function() { o2jse.ctrl.f(this); };
                            newEl.onblur       = function() { o2jse.ctrl.b(this); };
                            if (defObj.p.puexp) {
                                newEl.onmouseover = function(e) { o2jse.pu.i(e); };
                                newEl.onmouseout  = function() { o2jse.pu.o(); };
                                }
                            ctrlObj.parentNode.replaceChild(newEl, ctrlObj);
                            ctrlObj = newEl;
                            }
                        // ____________________________________ Replace INPUT with DIV ___
                        else {
                            var newEl          = document.createElement("div");
                            newEl.style.width  = ctrlObj.style.width;
                            newEl.style.height = ctrlObj.style.height;
                            newEl.id           = ctrlObj.name;
                            newEl.o2           = defObj.p;
                            if (defObj.p.puexp) {
                                newEl.onmouseover = function(e) { o2jse.pu.i(e); };
                                newEl.onmouseout  = function() { o2jse.pu.o(); };
                                }
                            // _________ Disable focused ctrl: focus to fall-back ctrl ___
                            if ((document.activeElement.name == ctrlObj.name) &&
                                o2jse.ctrl.focusFallBack) {
                                o2jse.cmd.focus(o2jse.ctrl.focusFallBack);
                                }
                            ctrlObj.parentNode.replaceChild(newEl, ctrlObj);
                            ctrlObj = newEl;
                            }
                        newEl = null;
                        // ________________________ Restore ZOOM on control, if needed ___
                        if (defObj.p.z) {
                            ctrlObj.ondblclick = function(e) { o2jse.ctrl.zoom(e); };
                            }
                        }
                    // _________________________________________ Set value for ENABLED ___
                    if (defObj.e) {
                        // __________________________________________________ Set size ___
                        if (ctrlObj.maxLength != defObj.p.s) {
                            ctrlObj.maxLength = defObj.p.s;
                            }
                        // ________________________ Set value (if not changed by user) ___
                        if (!ctrlObj.inEdit && modFields.value.indexOf(defObj.p.c) < 0) {
                            if (ctrlObj.value != defObj.d) {
                                ctrlObj.value = defObj.d;
                                }
                            }
                        }
                    // ________________________________________ Set value for DISABLED ___
                    else {
                        if (ctrlObj.innerText != defObj.d) {
                            ctrlObj.innerText = defObj.d;
                            }
                        }
                    // _________________________________________________ Set alignment ___
                    if (ctrlObj.style.textAlign != defObj.a) {
                        ctrlObj.style.textAlign = defObj.a;
                        }
                    // _______________________________________________ Set style class ___
                    if (ctrlObj.className != defObj.s) {
                        ctrlObj.className = defObj.s;
                        }
                    }
                else {
                    ctrlObj.style.display = "none";
                    }
                break;
            // ================================================================== TEXT ===
            case "text":
                if (defObj.v) {
                    // ______________________________________ Rich Text Editor WYSIWYG ___
                    if (defObj.p.rte) {
                        // ____________________________________________ Set visibility ___
                        ctrlObj.parentNode.style.display = "";
                        // ___________________________________________ Set style class ___
                        if (ctrlObj.className != defObj.p.cssc) {
                            ctrlObj.className = defObj.p.cssc;
                            }
                        // __________________________________________________ Set text ___
                        if (ctrlObj.value != defObj.text) {
                            ctrlObj.value = defObj.text;
                            }
                        }
                    // _____________________________________________ Standard textarea ___
                    else {
                        // ____________________________________________ Set visibility ___
                        ctrlObj.style.display = "";
                        ctrlObj.style.width   = defObj.w + 'px';
                        ctrlObj.style.height  = defObj.h + 'px';
                        if (defObj.p.pT != 'tab') {
                            ctrlObj.parentNode.style.left = defObj.x + 'px';
                            ctrlObj.parentNode.style.top  = defObj.y + 'px';
                            }
                        // ______________________________________________ Set editable ___
                        var zoom     = defObj.p.z;
                        var disabled = ctrlObj.nodeName != "TEXTAREA";
                        if (disabled == defObj.e) {
                            if (defObj.e) {
                                var newEl          = document.createElement("textarea");
                                newEl.style.width  = ctrlObj.style.width;
                                newEl.style.height = ctrlObj.style.height;
                                newEl.name         = ctrlObj.id;
                                newEl.id           = ctrlObj.id;
                                newEl.o2           = defObj.p;
                                newEl.onchange     = function() { o2jse.ctrl.c(this); };
                                newEl.onfocus      = function() { o2jse.ctrl.f(this); };
                                newEl.onblur       = function() { o2jse.ctrl.b(this); };
                                if (defObj.p.puexp) {
                                    newEl.onmouseover = function(e) { o2jse.pu.i(e); };
                                    newEl.onmouseout  = function() { o2jse.pu.o(); };
                                    }
                                ctrlObj.parentNode.replaceChild(newEl, ctrlObj);
                                ctrlObj = newEl;
                                }
                            else {
                                var newEl          = document.createElement("div");
                                newEl.style.width  = ctrlObj.style.width;
                                newEl.style.height = ctrlObj.style.height;
                                newEl.id           = ctrlObj.name;
                                newEl.o2           = defObj.p;
                                if (defObj.p.puexp) {
                                    newEl.onmouseover = function(e) { o2jse.pu.i(e); };
                                    newEl.onmouseout  = function() { o2jse.pu.o(); };
                                    }
                                // _____ Disable focused ctrl: focus to fall-back ctrl ___
                                if ((document.activeElement.name == ctrlObj.name) &&
                                    o2jse.ctrl.focusFallBack) {
                                    o2jse.cmd.focus(o2jse.ctrl.focusFallBack);
                                    }
                                ctrlObj.parentNode.replaceChild(newEl, ctrlObj);
                                ctrlObj = newEl;
                                }
                            newEl = null;
                            // ____________________ Restore ZOOM on control, if needed ___
                            if (defObj.p.z) {
                                ctrlObj.ondblclick = function(e) { o2jse.ctrl.zoom(e); };
                                }
                            }
                        // _____________________________________ Set value for ENABLED ___
                        if (defObj.e) {
                            // ____________________ Set value (if not changed by user) ___
                            if (!ctrlObj.inEdit &&
                                modFields.value.indexOf(defObj.p.c) < 0) {
                                if (ctrlObj.value != defObj.text) {
                                    ctrlObj.value = defObj.text;
                                    }
                                }
                            }
                        // ____________________________________ Set value for DISABLED ___
                        else {
                            if (ctrlObj.innerText != defObj.text) {
                                ctrlObj.innerText = defObj.text;
                                }
                            }
                        // ___________________________________________ Set style class ___
                        if (ctrlObj.className != defObj.s) {
                            ctrlObj.className = defObj.s;
                            }
                        }
                    }
                else {
                    if (defObj.p.rte) {
                        ctrlObj.parentNode.style.display = "none";
                        }
                    else {
                        ctrlObj.style.display = "none";
                        }
                    }
                break;
            // ============================================================= CHECK-BOX ===
            case "checkbox":
                // ____________________________________________________ Set visibility ___
                if (defObj.v) {
                    ctrlObj.style.display = "";
                    if (defObj.p.pT != 'tab') {
                        ctrlObj.parentNode.style.left = defObj.x + 'px';
                        ctrlObj.parentNode.style.top  = defObj.y + 'px';
                        }
                    // __________________________________________________ Set editable ___
                    if (ctrlObj.disabled == defObj.e) {
                        if (defObj.e) {
                            ctrlObj.disabled = false;
                            ctrlObj.onclick = function() { o2jse.cb.c(this, defObj.vS); };
                            }
                        else {
                            ctrlObj.disabled = true;
                            ctrlObj.onclick  = null;
                            }
                        }
                    // _______________________________________________ Set style class ___
                    if (ctrlObj.className != defObj.s) {
                        ctrlObj.className = defObj.s;
                        }
                    // ___________________________________________________ Set checked ___
                    if (ctrlObj.checked != defObj.d) {
                        ctrlObj.checked = defObj.d;
                        }
                    }
                else {
                    ctrlObj.style.display = "none";
                    }
                break;
            // ========================================================= LIST-COMBOBOX ===
            case "listcombo":
                // ========================================================== List-box ===
                if (defObj.boxtype != "lookup") {
                    divObj = document.getElementById(defObj.i);
                    // _______________________________________________________ Visible ___
                    if (defObj.v) {
                        divObj.style.display = "";
                        divObj.style.width   = defObj.w + 'px';
                        divObj.style.height  = defObj.h + 'px';
                        if (defObj.p.pT != 'tab') {
                            divObj.parentNode.style.left = defObj.x + 'px';
                            divObj.parentNode.style.top  = defObj.y + 'px';
                            }
                        // ____________________________________________ Active control ___
                        var zoom = defObj.p.z;
                        if (defObj.e) {
                            divObj.onclick = function(e) { o2jse.lb.m(e); };
                            }
                        // __________________________________________________ Disabled ___
                        else {
                            divObj.onclick     = null;
                            }
                        // _________________________ Reload listbox options from items ___
                        var opts = divObj.getElementsByTagName("div");
                        var icode;
                        var itext;
                        divObj.innerHTML = "";
                        for (var i = 0; i < defObj.items.length; i++) {
                            icode      = defObj.items[i][0];
                            itext      = defObj.items[i][1];
                            opts[i]    = o2jse.createEl(divObj, "DIV", "", itext);
                            opts[i].id = "jxdata:" + icode;
                            if (icode == defObj.d) {
                               opts[i].className = "jxlsr";
                               }
                            }
                        // ________________________ Set value (if not changed by user) ___
                        if (ctrlObj.value != defObj.d &&
                            modFields.value.indexOf(defObj.p.c) < 0) {
                            ctrlObj.value = defObj.d;
                            }
                        // ___________________________________________ Set style class ___
                        if (divObj.className != defObj.s) {
                            divObj.className = defObj.s;
                            }
                        }
                    // _____________________________________________________ Invisible ___
                    else {
                        divObj.style.display = "none";
                        }
                    }
                // ==================================================== Lookup control ===
                else {
                    var descField = document.getElementById(defObj.i + "_desc");
                    o2jse.ctrl.init(descField);
                    // _______________________________________________________ Visible ___
                    if (defObj.v) {
                        descField.style.display = "";
                        descField.style.width   = defObj.w + 'px';
                        descField.style.height  = defObj.h + 'px';
                        if (defObj.p.pT != 'tab') {
                            descField.parentNode.style.left = defObj.x + 'px';
                            descField.parentNode.style.top  = defObj.y + 'px';
                            }
                        var disabled                    = descField.nodeName != "INPUT";
                        if (disabled == defObj.e) {
                            // ________________________________ Replace DIV with INPUT ___
                            if (defObj.e) {
                                var newEl          = document.createElement("input");
                                newEl.style.width  = descField.style.width;
                                newEl.style.height = descField.style.height;
                                newEl.id           = descField.id;
                                newEl.o2           = defObj.p;
                                descField.onkeydown = function(e){ o2jse.lu.k(e, this); };
                                descField.onclick   = function() { o2jse.lu.ck(this); };
                                descField.onpaste   = function() { o2jse.lu.p(this); };
                                descField.onfocus   = function() { o2jse.lu.f(this); };
                                descField.onblur    = function() { o2jse.lu.b(this); };
                                if (defObj.p.puexp) {
                                    descField.onmouseover = function(e){ o2jse.pu.i(e); };
                                    descField.onmouseout  = function() { o2jse.pu.o(); };
                                    }
                                descField.parentNode.replaceChild(newEl, descField);
                                descField = newEl;
                                }
                            // ________________________________ Replace INPUT with DIV ___
                            else {
                                var newEl          = document.createElement("div");
                                newEl.style.width  = descField.style.width;
                                newEl.style.height = descField.style.height;
                                newEl.id           = descField.id;
                                newEl.o2           = defObj.p;
                                if (defObj.p.puexp) {
                                    newEl.onmouseover = function(e){ o2jse.pu.i(e); };
                                    newEl.onmouseout  = function() { o2jse.pu.o(); };
                                    }
                                // _____ Disable focused ctrl: focus to fall-back ctrl ___
                                if ((document.activeElement.id == descField.id) &&
                                    o2jse.ctrl.focusFallBack) {
                                    o2jse.cmd.focus(o2jse.ctrl.focusFallBack);
                                    }
                                descField.parentNode.replaceChild(newEl, descField);
                                descField = newEl;
                                }
                            newEl = null;
                            // ____________________ Restore ZOOM on control, if needed ___
                            if (defObj.p.z) {
                                descField.ondblclick = function(e){ o2jse.ctrl.zoom(e); };
                                }
                            }
                        // ____________________________________________ Active control ___
                        var zoom = defObj.p.z;
                        if (defObj.e) {
                            descField.style.cursor = "pointer";
                            if (defObj.dyn) {
                                descField.o2.dyn = true;
                                }
                            else {
                                descField.o2.items = defObj.items;
                                descField.o2.dyn   = false;
                                }
                            descField.o2.rows   = defObj.p.rows;
                            descField.onkeydown = function(e) { o2jse.lu.k(e, this); };
                            descField.onclick   = function() { o2jse.lu.ck(this); };
                            descField.onpaste   = function() { o2jse.lu.p(this); };
                            descField.onfocus   = function() { o2jse.lu.f(this); };
                            descField.onblur    = function() { o2jse.lu.b(this); };
                            }
                        // __________________________________________________ Disabled ___
                        else {
                            descField.style.cursor = "default";
                            o2jse.lu.listOff(descField);
                            // __________________________ Disactivate drop-down button ___
                            if (descField.dropDownActive) {
                                o2jse.removeEl(descField.parentNode.
                                                getElementsByTagName("div")[0]);
                                descField.dropDownActive = false;
                                }
                            }
                        // ________ Check if control has been edited since last submit ___
                        if (modFields.value.indexOf(defObj.p.c) < 0) {
                            // _____________________________________________ Set value ___
                            if (ctrlObj.value != defObj.d) {
                                ctrlObj.value = defObj.d;
                                delete descField.saveValue;
                                }
                            // _________________________________ Set value for ENABLED ___
                            if (defObj.e) {
                                // _____________________________ Set description value ___
                                if (descField.value != defObj.desc && !descField.inEdit) {
                                    descField.value = defObj.desc;
                                    descField.scrollLeft = 0;
                                    }
                                }
                            // ________________________________ Set value for DISABLED ___
                            else {
                                if (descField.innerText != defObj.d) {
                                    descField.innerText = defObj.desc;
                                    }
                                }
                            }
                        // ___________________________________________ Set style class ___
                        if (descField.className != defObj.p.cssf) {
                            descField.className = defObj.p.cssf;
                            }
                        }
                    // _____________________________________________________ Invisible ___
                    else {
                        descField.style.display = "none";
                        }
                    }
                break;
            // ================================================================ BUTTON ===
            case "button":
                // ____________________________________________________ Set visibility ___
                if (defObj.v) {
                    ctrlObj.style.display = "";
                    ctrlObj.style.width   = defObj.w + 'px';
                    ctrlObj.style.height  = defObj.h + 'px';
                    if (defObj.p.pT != 'tab') {
                        ctrlObj.parentNode.style.left = defObj.x + 'px';
                        ctrlObj.parentNode.style.top  = defObj.y + 'px';
                        }
                    // __________________________________________________ Set editable ___
                    if (defObj.e) {
                        ctrlObj.onclick  = function() { o2jse.ctrl.btnExe(this); };
                        ctrlObj.onfocus  = function() { o2jse.ctrl.f(this); };
                        ctrlObj.onblur   = function() { o2jse.ctrl.b(this); };
                        ctrlObj.tabIndex = "0";
                        }
                    else {
                        ctrlObj.onclick     = null;
                        ctrlObj.onfocus     = null;
                        ctrlObj.onblur      = null;
                        ctrlObj.removeAttribute("tabIndex");
                        }
                    // _______________________________________________ Set style class ___
                    if (ctrlObj.className != defObj.s) {
                        ctrlObj.className = defObj.s;
                        }
                    // ______________________________________________________ Set text ___
                    if (ctrlObj.innerHTML != defObj.l) {
                        ctrlObj.innerHTML = defObj.l;
                        }
                    }
                else {
                    ctrlObj.style.display = "none";
                    }
                break;
            // ================================================================= IMAGE ===
            case "img":
                // ____________________________________________________ Set visibility ___
                if (defObj.v) {
                    ctrlObj.style.display = "";
                    ctrlObj.style.width   = defObj.w + 'px';
                    ctrlObj.style.height  = defObj.h + 'px';
                    if (defObj.p.pT != 'tab') {
                        ctrlObj.parentNode.style.left = defObj.x + 'px';
                        ctrlObj.parentNode.style.top  = defObj.y + 'px';
                        }
                    // _____________________________________________________ Set image ___
                    if (ctrlObj.src != defObj.src) {
                        ctrlObj.src = defObj.src;
                        }
                    // _________________________________________________ Set clickable ___
                    if (defObj.e) {
                        if (!ctrlObj.onclick) {
                            ctrlObj.onclick = function() { o2jse.ctrl.btnExe(this); };
                            ctrlObj.style.cursor = "pointer";
                            }
                        if (defObj.srcover && !ctrlObj.onmouseover) {
                            ctrlObj.onmouseover = function() { this.src = defObj.srcover;
                                                             };
                            ctrlObj.onmouseout  = function() { this.src = defObj.src; };
                            }
                        }
                    else {
                        ctrlObj.onclick      = null;
                        ctrlObj.onmouseover  = null;
                        ctrlObj.onmouseout   = null;
                        ctrlObj.style.cursor = "default";
                        }
                    // _______________________________________________ Set style class ___
                    if (ctrlObj.className != defObj.s) {
                        ctrlObj.className = defObj.s;
                        }
                    // ______________________________________________________ Set text ___
                    if (ctrlObj.title != defObj.l) {
                        ctrlObj.alt   = defObj.l;
                        ctrlObj.title = defObj.l;
                        }
                    }
                else {
                    ctrlObj.style.display = "none";
                    }
                break;
            // ================================================================== AREA ===
            case "html":
                // ____________________________________________________ Set visibility ___
                if (defObj.v) {
                    ctrlObj.style.display = "";
                    ctrlObj.style.width   = defObj.w + 'px';
                    ctrlObj.style.height  = defObj.h + 'px';
                    if (defObj.p.pT != 'tab') {
                        ctrlObj.parentNode.style.left = defObj.x + 'px';
                        ctrlObj.parentNode.style.top  = defObj.y + 'px';
                        }
                    // _______________________________________________ Set style class ___
                    if (ctrlObj.className != defObj.s) {
                        ctrlObj.className = defObj.s;
                        }
                    // _____________________________________________________ Set value ___
                    if (ctrlObj.innerHTML != defObj.code) {
                        ctrlObj.innerHTML = defObj.code;
                        jxjs.runScripts(ctrlObj);
                        }
                    }
                else {
                    ctrlObj.style.display = "none";
                    }
                break;
            // ============================================================== TREEVIEW ===
            case "tree":
                // ____________________________________________________ Set visibility ___
                if (defObj.v) {
                    ctrlObj.style.display = "";
                    ctrlObj.style.width   = defObj.w + 'px';
                    ctrlObj.style.height  = defObj.h + 'px';
                    if (defObj.p.pT != 'tab') {
                        ctrlObj.parentNode.style.left = defObj.x + 'px';
                        ctrlObj.parentNode.style.top  = defObj.y + 'px';
                        }
                    // _______________________________________________ Set style class ___
                    if (ctrlObj.className != defObj.s) {
                        ctrlObj.className = defObj.s;
                        }
                    // _________________________________________________ Set HTML code ___
                    if (ctrlObj.innerHTML != defObj.code) {
                        ctrlObj.innerHTML = defObj.code;
                        }
                    }
                else {
                    ctrlObj.style.display = "none";
                    }
                break;
            // ========================================================= IMAGES-LISTER ===
            case "imglist":
                // ____________________________________________________ Set visibility ___
                if (defObj.v) {
                    ctrlObj.style.display = "";
                    ctrlObj.style.width   = defObj.w + 'px';
                    ctrlObj.style.height  = defObj.h + 'px';
                    if (defObj.p.pT != 'tab') {
                        ctrlObj.parentNode.style.left = defObj.x + 'px';
                        ctrlObj.parentNode.style.top  = defObj.y + 'px';
                        }
                    // _______________________________________________ Set style class ___
                    if (ctrlObj.className != defObj.s) {
                        ctrlObj.className = defObj.s;
                        }
                    // _________________________________________________ Set HTML code ___
                    if (ctrlObj.innerHTML != defObj.code) {
                        ctrlObj.innerHTML = defObj.code;
                        }
                    }
                else {
                    ctrlObj.style.display = "none";
                    }
                break;
            // ========================================================== PROGRESS-BAR ===
            case "progress":
                // ____________________________________________________ Set visibility ___
                if (defObj.v) {
                    ctrlObj.style.display = "";
                    ctrlObj.style.width   = defObj.w + 'px';
                    ctrlObj.style.height  = defObj.h + 'px';
                    if (defObj.p.pT != 'tab') {
                        ctrlObj.parentNode.style.left = defObj.x + 'px';
                        ctrlObj.parentNode.style.top  = defObj.y + 'px';
                        }
                    // _______________________________________________ Set style class ___
                    if (ctrlObj.className != defObj.s) {
                        ctrlObj.className = defObj.s;
                        }
                    // _____________________________________________________ Set value ___
                    o2jse.progress.set(ctrlObj.id, defObj.vl);
                    // ______________________________________________ Activate refresh ___
                    if (defObj.a) {
                        o2jse.progress.start();
                        }
                    }
                else {
                    ctrlObj.style.display = "none";
                    }
                break;
            // =============================================================== FLOWBOX ===
            case "flowbox":
                // ____________________________________________________ Set visibility ___
                if (defObj.v) {
                    ctrlObj.style.display = "";
                    ctrlObj.style.width   = defObj.w + 'px';
                    ctrlObj.style.height  = defObj.h + 'px';
                    if (defObj.p.pT != 'tab') {
                        ctrlObj.parentNode.style.left = defObj.x + 'px';
                        ctrlObj.parentNode.style.top  = defObj.y + 'px';
                        }
                    // _______________________________________________ Set style class ___
                    if (ctrlObj.className != defObj.s) {
                        ctrlObj.className = defObj.s;
                        }
                    }
                else {
                    ctrlObj.style.display = "none";
                    }
                break;
            // ================================================================= FRAME ===
            case "frame":
                // ____________________________________________________ Set visibility ___
                if (defObj.v) {
                    ctrlObj.style.display = "";
                    // _______________________________ Size is set in the internal DIV ___
                    intDiv                = ctrlObj.getElementsByTagName('div')[0];
                    intDiv.style.width    = defObj.w + 'px';
                    intDiv.style.height   = defObj.h + 'px';
                    // _______________________________________________ Set style class ___
                    if (ctrlObj.className != defObj.s) {
                        ctrlObj.className = defObj.s;
                        }
                    }
                else {
                    ctrlObj.style.display = "none";
                    }
                break;
            // ============================================================= NAVIGATOR ===
            case "navigator":
                // ________________________________________________________ Get object ___
                var ctrlObjDiv = ctrlObj.parentNode;
                // __________________________________________________ Check visibility ___
                if (defObj.v) {
                    ctrlObjDiv.style.display = "";
                    if (defObj.p.pT != 'tab') {
                        ctrlObj.style.width   = defObj.w + 'px';
                        ctrlObj.style.height  = defObj.h + 'px';
                        ctrlObjDiv.style.left = defObj.x + 'px';
                        ctrlObjDiv.style.top  = defObj.y + 'px';
                        }
                    // ________________________________________ Navigator HTML content ___
                    if (defObj.code) {
                        // _______________________________________________ Set content ___
                        ctrlObjDiv.innerHTML = defObj.code;
                        }
                    }
                else {
                    ctrlObjDiv.style.display = "none";
                    }
                break;
            // =========================================================== FILE UPLOAD ===
            case "file":
                // ________________________________________________________ Get object ___
                var ctrlObjDiv = ctrlObj.parentNode.parentNode;
                // __________________________________________________ Check visibility ___
                if (defObj.v) {
                    ctrlObjDiv.style.display = "";
                    ctrlObjDiv.style.width   = defObj.w + 'px';
                    ctrlObjDiv.style.height  = defObj.h + 'px';
                    if (defObj.p.pT != 'tab') {
                        ctrlObjDiv.parentNode.style.left = defObj.x + 'px';
                        ctrlObjDiv.parentNode.style.top  = defObj.y + 'px';
                        }
                    }
                else {
                    ctrlObjDiv.style.display = "none";
                    }
                break;
                }
        jxjs.dbg_ctrl = "Processed " + defObj.i + " (" + defObj.t + ")";
        }

    };


/**
 * Manage view selection information in FULL-AJAX mode
 *
 * @param {String}  formName
 * @param {Integer} seleValue
 */
jxv = function(formName, seleValue) {

    // _____________________________________________ If selection field already exists ___
    if (o2jse.infoForm[formName]) {
        o2jse.infoForm[formName].value = seleValue;
        }
    else {
        seleField = document.createElement("input");
        seleField.setAttribute("type", "hidden");
        seleField.setAttribute("name", formName);
        seleField.setAttribute("value", seleValue);
        o2jse.infoForm.appendChild(seleField);
        }
    };


/**
 * Full page code replacement in FULL-AJAX mode
 *
 * @param {String} pageCode
 */
jxp_old = function(pageCode) {

    // _______________________________________________________ Get the actual HTML tag ___
    var oHTML       = document.getElementsByTagName('html')[0];
    // _________________________________________________________ Create a new HTML tag ___
    var nHTML       = document.createElement("html");
    // ________________________________________ Insert page code as HTML tag innerHTML ___
    nHTML.innerHTML = pageCode;
    // _________________________________________ Replace the old node with the new one ___
    oHTML.parentNode.replaceChild(nHTML, oHTML);
    oHTML = null;
    // __________________________________________________ Run scripts inside page code ___
    o2jse.started          = false;
    o2jse.fastMsg.frameObj = null;
    if (o2jse.devName && o2jse.lab) {
        o2jse.lab.contWin = null;
        }
    jxjs.runScripts(nHTML);

    };


jxp = function(pageCode) {

    // _______________________________________________________ Get the actual BODY tag ___
    var oBody            = document.getElementsByTagName('body')[0];
    // _________________________________________________________ Create a new BODY tag ___
    var nBody            = document.createElement('body');
    // ________________________________________ Insert page code as BODY tag innerHTML ___
    nBody.innerHTML      = pageCode;
    nBody.className      = "o2_sfondo";
    nBody.onload         = o2jse.init;
    nBody.onBeforeUnload = o2jse.cmd.showClock;
    // _________________________________________ Replace the old node with the new one ___
    oBody.parentNode.replaceChild(nBody, oBody);
    oBody = null;
    // __________________________________________________ Run scripts inside page code ___
    o2jse.started          = false;
    o2jse.fastMsg.frameObj = null;
    if (o2jse.devName && o2jse.lab) {
        o2jse.lab.contWin = null;
        }
    jxjs.runScripts(nBody);

    };


/**
 * Server-side error reporting in FULL-AJAX mode
 *
 * @param {String} errCode
 */
jxe = function(errCode) {

    contDiv           = o2jse.createEl(document.getElementById("jxjsinsert"), "div");
    contDiv.innerHTML = errCode;
    o2jse.submitting  = false;
    o2jse.cliMode     = false;

    };


/**
 * Menu objects collector. Provides existing menus list and tools.
 */
o2jse.menu = {

    menuList     : [],   /* List of defined menu objects                                */
    openMenus    : [],   /* List of actually open menus                                 */
    timeout      : null, /* Timeout for menu closing                                    */
    lastBarItem  : null, /* Last selected element in menu bar                           */
    docByMenu    : false /* If is active a help request by menu                         */

    };


/**
 * Return a new instance of menu object and add it to o2jse.menu.menuList array.
 * Menu object will be available as o2jse.menu.menuList[menuId].
 *
 * @param {String} menuId   Menu name - unique identifier
 * @param {String} label    Label displayed on menu item
 */
o2jse.menu.addMenu = function(menuId, label) {

    var newMenu = {

        id     : menuId, /* Menu unique ID                                              */
        label  : label,  /* Label in menu item                                          */
        shown  : false,  /* If menu is actually displayed                               */
        items  : [],     /* Contained item objects list                                 */
        outBox : null,   /* HTML DIV element for showing menu shadow and whole content  */
        inBox  : null,   /* HTML DIV element containing menu                            */
        frame  : null,   /* HTML TABLE element containing items                         */
        width  : 0,      /* Out-box original width                                      */
        height : 0       /* Out-box original height                                     */

        };

    newMenu.outBox                = document.createElement("DIV");
    newMenu.outBox.className      = "winshadow";
    newMenu.outBox.style.position = "fixed";
    newMenu.outBox.onmouseover    = function() {
                                       if (o2jse.menu.timeout) {
                                           clearTimeout(o2jse.menu.timeout);
                                           }
                                       };
    newMenu.outBox.onmouseout     = o2jse.menu.closeByTime;
    newMenu.inBox                 = o2jse.createEl(newMenu.outBox, "DIV", "o2menu");
    newMenu.frame                 = o2jse.createEl(newMenu.inBox, "TABLE");
    newMenu.frame.cellPadding     = "0";
    newMenu.frame.cellSpacing     = "0";


    /**
     * Adds a new item to an existing menu.
     * If new item is a menu, related menu object will be created too.
     *
     * NOTE: itemType "R" is intended to to add arbitrayi rows to menu, as labels or more
     *       complex HTML structures.
     *
     * @param string itemType   Type of item in [M]enu,[P]rogram,[U]rl,[S]eparator
     * @param string elementId  Item unique identifier
     * @param string labelTxt   Text to be displayed as menu text
     * @param string dataStr    Data to be used by menu (Program name, Url, ...)
     * @param string imgFile    Image file to be displayed as menu icon
     */
    newMenu.addItem = function(itemType, elementId, labelTxt, dataStr, imgFile) {

        itemType       = String(itemType).toUpperCase();
        var newItem    = {};
        var newItemId  = newMenu.items.length;
        newItem.row    = newMenu.frame.insertRow(-1);
        newItem.row.o2 = {c         : elementId,
                          cT        : "menuitem",
                          itemType  : itemType,
                          prgName   : (itemType == "P" ? dataStr : ""),
                          itemLabel : labelTxt,
                          itemImg   : imgFile,
                          fret      : true};
        switch (itemType) {
            case "P":
                buildActiveItem();
                newItem.row.onclick = function() {
                                          o2jse.menu.
                                           openMenus[o2jse.menu.openMenus.length] =
                                                                      { label: labelTxt };
                                          o2jse.cmd.run(dataStr);
                                          };
                newItem.labelCell.colSpan = "2";
                break;
            case "M":
                if (!o2jse.menu.menuList[elementId]) {
                    o2jse.menu.addMenu(elementId, labelTxt);
                    }
                buildActiveItem();
                newItem.arrowCell           = newItem.row.insertCell(-1);
                newItem.arrowCell.className = "o2menuArrow";
                break;
            case "S":
                newItem.row.className       = "o2menuSep";
                newItem.labelCell           = newItem.row.insertCell(-1);
                newItem.labelCell.colSpan   = "3";
                newItem.labelCell.className = "o2menuSep";
                break;
            case "J":
                buildActiveItem();
                if (typeof dataStr == "function") {
                    newItem.row.onclick = dataStr;
                    }
                else {
                    newItem.row.onclick = new Function("event", dataStr);
                    }
                newItem.labelCell.colSpan = "2";
                break;
            case "U":
                buildActiveItem();
                newItem.row.onclick       = function() {
                                                window.location.href = dataStr;
                                                };
                newItem.labelCell.colSpan = "2";
                break;
            case "R":
//                newItem.row.className = "o2menuItem";
                newItem.row.innerHTML = labelTxt;
                break;
            default:
                buildActiveItem();
                newItem.row.onclick       = function() { alert(dataStr); };
                newItem.labelCell.colSpan = "2";
                break;
            }
        newMenu.items[newItemId] = newItem;


        /**
         * Sets item row as "active" and sets image and label
         *
         */
        function buildActiveItem() {

            newItem.row.className   = "o2menuItem";
            newItem.row.itemType    = itemType;
            newItem.row.elementId   = elementId;
            newItem.row.parentId    = newMenu.id;
            newItem.row.onmouseover = newMenu.onMenuItem;
            newItem.row.onmouseout  = function() { this.className = "o2menuItem"; };
            newItem.imgCell         = newItem.row.insertCell(-1);
            if (imgFile) {
                newItem.imgCell.className = "o2menuImg";
                newItem.img               = o2jse.createEl(newItem.imgCell, "IMG");
                newItem.img.src           = imgFile;
                }
            newItem.labelCell           = newItem.row.insertCell(-1);
            newItem.labelCell.className = "o2menuLabel";
            newItem.labelCell.innerHTML = labelTxt;

            };

        };


    /**
     * Removes all items from within the menu context.
     *
     */
    newMenu.clear = function() {

        newMenu.inBox.innerHTML   = "";
        newMenu.frame             = o2jse.createEl(newMenu.inBox, "TABLE");
        newMenu.frame.cellPadding = "0";
        newMenu.frame.cellSpacing = "0";

        };


    /**
     * Overrides items loading method replacing the whole menu context.
     * Can accept both object and text (HTML code).
     *
     * @param mix menuContext   Context to be inserted in the menu
     */
    newMenu.context = function(menuContext) {

        newMenu.inBox.innerHTML = "";
        newMenu.frame           = null;
        if (typeof menuContext == "object") {
            newMenu.inBox.appendChild(menuContext);
            }
        else {
            newMenu.inBox.innerHTML = menuContext;
            }

        };


    /**
     * Shows menu.
     *
     * @param object posObj   Object containing .x and .y properties for position
     */
    newMenu.show = function(posObj, parentObj, barObj) {

        o2jse.elBody.appendChild(newMenu.outBox);
        // _________________________________________________ Menu opened from menu-bar ___
        if (barObj) {
            if (posObj.x < (o2jse.cli.width - newMenu.outBox.offsetWidth)) {
                newMenu.outBox.style.left = posObj.x + "px";
                }
            else {
                newMenu.outBox.style.left = (posObj.x + barObj.parentNode.offsetLeft +
                                             barObj.offsetWidth -
                                             newMenu.outBox.offsetWidth) + "px";
                }
            if (posObj.y < (o2jse.cli.height - newMenu.outBox.offsetHeight)) {
                newMenu.outBox.style.top = posObj.y + "px";
                }
            else {
                newMenu.outBox.style.top = (o2jse.cli.height - newMenu.outBox.offsetHeight
                                            - 10) + "px";
                }
            if (o2jse.menuStyle != 'T') {
                if (newMenu.inBox.className.indexOf('o2menuVert') < 0) {
                    newMenu.inBox.className+= ' o2menuVert';
                    }
                }
            }
        // ___________________________________________________ Menu opened from a menu ___
        else if (parentObj) {
            if (posObj.x < (o2jse.cli.width - newMenu.outBox.offsetWidth)) {
                newMenu.outBox.style.left = posObj.x + "px";
                }
            else {
                newMenu.outBox.style.left = (posObj.x - parentObj.offsetWidth -
                                             newMenu.outBox.offsetWidth) + "px";
                }
            if (posObj.y < (o2jse.cli.height - newMenu.outBox.offsetHeight)) {
                newMenu.outBox.style.top = posObj.y + "px";
                }
            else {
                newMenu.outBox.style.top = (o2jse.cli.height - newMenu.outBox.offsetHeight
                                            - 10) + "px";
                }
            if (newMenu.inBox.className.indexOf('o2menuFloat') < 0) {
                newMenu.inBox.className+= ' o2menuFloat';
                }
            }
        // ________________________________________ Menu opened from right click point ___
        else {
            if (posObj.x < (o2jse.cli.width - newMenu.outBox.offsetWidth)) {
                newMenu.outBox.style.left = posObj.x + "px";
                }
            else {
                newMenu.outBox.style.left = (o2jse.cli.width - newMenu.outBox.offsetWidth
                                             - 10) + "px";
                }
            if (posObj.y < (o2jse.cli.height - newMenu.outBox.offsetHeight)) {
                newMenu.outBox.style.top = posObj.y + "px";
                }
            else {
                newMenu.outBox.style.top = (o2jse.cli.height - newMenu.outBox.offsetHeight
                                            - 10) + "px";
                }
            if (newMenu.inBox.className.indexOf('o2menuFloat') < 0) {
                newMenu.inBox.className+= ' o2menuFloat';
                }
            }
        // ____________________________________________ Vertical scroll for high menus ___
        if (newMenu.outBox.offsetHeight > (o2jse.cli.height - 20)) {
            newMenu.outBox.style.height    = (o2jse.cli.height - 20) + "px";
            newMenu.outBox.style.top       = "10px";
            newMenu.outBox.style.overflowY = "scroll";
            newMenu.outBox.style.overflowX = "hidden";
            }
        o2jse.menu.openMenus[o2jse.menu.openMenus.length] = newMenu;
        newMenu.shown = true;

        };


    /**
     * Event handler for onMouseOver events on menu items
     *
     */
    newMenu.onMenuItem = function() {

        this.className = "o2menuItemOn";
        o2jse.menu.closeDownNode(this.parentId);
        if (this.itemType == "M") { // __________________________________ Open submenu ___
            var posObj   = {x : 0, y : 0};
            //                TR    TBODY TABLE(frame) DIV(inBox) DIV(outBox)
            var menuFrom   = this.parentNode.parentNode.parentNode;
            var shadowFrom = menuFrom.parentNode;
            posObj.x       = shadowFrom.offsetLeft +
                             menuFrom.offsetLeft +
                             menuFrom.offsetWidth - 1;
            posObj.y       = shadowFrom.offsetTop + menuFrom.offsetTop + this.offsetTop;
            o2jse.menu.menuList[this.elementId].show(posObj, menuFrom);
            }

        };

    o2jse.menu.menuList[menuId] = newMenu;
    return newMenu;

    };


/**
 * Show/hide menu bar when style is "Hide"
 *
 */
o2jse.menu.menuBtn = function() {

    menuBar = document.getElementById('jxMenuBar');
    menuBtn = document.getElementById('jxMenuButton');
    if (menuBar.style.display == 'none') {
        menuBtn.className     = 'o2menuClose';
        menuBar.style.display = 'block';
        }
    else {
        menuBtn.className     = 'o2menuButton';
        menuBar.style.display = 'none';
        }

    };


/**
 * Event handler for onMouseOver events on menu bar items
 *
 */
o2jse.menu.onOverBarItem = function(targetObj, itemType, elementId) {

    if (o2jse.menu.openMenus.length) {
        o2jse.menu.activateBarItem(targetObj, itemType, elementId);
        }

    };


/**
 * Event handler for onMouseOver events on menu bar items
 *
 */
o2jse.menu.onOutBarItem = function(targetObj, itemType, elementId) {

    o2jse.menu.lastBarItem = targetObj;

    };


/**
 * Event handler for onClick events on menu bar items
 *
 */
o2jse.menu.onClickBarItem = function(targetObj, itemType, elementId, dataStr) {

    switch (itemType) {
        case "M":
            if (o2jse.menu.openMenus.length) {
                for (var menuId in o2jse.menu.openMenus) {
                    var menu = o2jse.menu.openMenus[menuId];
                    if ((menu.id == elementId) && menu.shown) {
                        targetObj.className = "o2menuItem";
                        o2jse.menu.closeAll();
                        return;
                        }
                    }
                }
            break;
        case "P":
            o2jse.menu.openMenus[o2jse.menu.openMenus.length] =
                                                            { id: targetObj.textContent };
            o2jse.cmd.run(dataStr);
            break;
        default:
            alert(itemType);
            break;
        }
    o2jse.menu.activateBarItem(targetObj, itemType, elementId);

    };


/**
 * Open sub menu for a menu-bar item
 *
 */
o2jse.menu.activateBarItem = function(targetObj, itemType, elementId) {

    if (o2jse.menu.lastBarItem) {
        o2jse.menu.lastBarItem.className = "o2menuItem";
        }
    if (o2jse.menu.timeout) {
        clearTimeout(o2jse.menu.timeout);
        }
    o2jse.menu.loadAppMenu();
    o2jse.menu.closeDownNode();
    targetObj.className = "o2menuItemOn";
    if (itemType == "M") { // ___________________________________________ Open submenu ___
        var posObj  = {x:0, y:0};
        var menuBar = targetObj.parentNode;
        if (o2jse.menuStyle == 'T') {
            posObj.x = targetObj.offsetLeft;
            posObj.y = menuBar.offsetTop + targetObj.offsetTop + targetObj.offsetHeight -
                       1;
            }
        else {
            posObj.x = targetObj.offsetLeft + targetObj.offsetWidth;
            posObj.y = menuBar.offsetTop - menuBar.scrollTop + targetObj.offsetTop;
            }
        o2jse.menu.menuList[elementId].show(posObj, null, targetObj);
        }
    else {
        o2jse.menu.openMenus[o2jse.menu.openMenus.length] = true;
        }


    };


/**
 * Closes all menus down a requested node
 *
 */
o2jse.menu.closeDownNode = function(menuId) {

    var openMenus = o2jse.menu.openMenus;
    if (openMenus.length > 0) {
        for (var ancestorId = openMenus.length; ancestorId > 0; ancestorId--) {
            ancMenu = openMenus[ancestorId - 1];
            if (ancMenu.id != menuId) {
                o2jse.removeEl(ancMenu.outBox);
                ancMenu.shown = false;
                openMenus.pop();
                }
            else {
                break;
                }
            }
        }

    };


/**
 * Activate timer for menus closing
 *
 */
o2jse.menu.closeByTime = function() {

    if (o2jse.menu.timeout) {
        clearTimeout(o2jse.menu.timeout);
        }
    o2jse.menu.timeout = setTimeout(o2jse.menu.closeAll, 1000);

    };


/**
 * Close all open menus
 *
 */
o2jse.menu.closeAll = function() {

    if (o2jse.menu.lastBarItem) {
        o2jse.menu.lastBarItem.className = "o2menuItem";
        }
    if (o2jse.menu.openMenus.length > 0) {
        for (var ancestorId = o2jse.menu.openMenus.length;
             ancestorId > 0;
             ancestorId--) {
            o2jse.removeEl(o2jse.menu.openMenus[ancestorId - 1].outBox);
            o2jse.menu.openMenus[ancestorId - 1].shown = false;
            o2jse.menu.openMenus.pop();
            }
        }
    o2jse.menu.openMenus = [];
    o2jse.win.showMenu   = false;

    };


/**
 * Load application menu if one is defined and if it hasn't been loaded yet
 *
 */
o2jse.menu.loadAppMenu = function() {

    if (o2jse.menu.appMainMenu && !o2jse.menu.menuList[o2jse.menu.appMainMenu]) {
        o2jse.menu.appMenu();
        }

    };



/* ================================== CONTEXT MENU ==================================== */


/**
 * Context menu object.
 * Single instance is created on page load and context is loaded each time object is
 * activated by method setOn().
 *
 * Menu is referenced both by o2jse.cMenu and o2jse.menu.menuList['o2cmenu'].
 *
 */
o2jse.cMenu           = o2jse.menu.addMenu("o2cmenu");
o2jse.cMenu.shown     = false; /* If context manu is actually displayed                 */
o2jse.cMenu.target    = null;  /* Object on which context menu is activated             */
o2jse.cMenu.partnersB = [];    /* Registered "partners" adding items before std ones    */
o2jse.cMenu.partnersA = [];    /* Registered "partners" adding items after std ones     */


/**
 * Load items and build menu context for displaying.
 * This function should be called in the onclick event handler for document or window
 * objects (see this file bottom).
 *
 * @param object eventObj   Event object passed by onclick event
 */
o2jse.cMenu.setOn = function(eventObj) {

    var stdEvent       = o2jse.event.std(eventObj);
    o2jse.cMenu.target = stdEvent.target;
    if (o2jse.cMenu.loadItems()) {
        stdEvent.stop();
        o2jse.cMenu.show(stdEvent);
        }
    return true;

    };


/**
 * Set context menu off and reset object context
 *
 */
o2jse.cMenu.setOff = function() {

    o2jse.removeEl(o2jse.menu.menuList["o2cmenu"].outBox);
    o2jse.menu.menuList["o2cmenu"].outBox = null;
    o2jse.menu.menuList["o2cmenu"].shown  = false;

    };

/**
 * Stops timeout running for menu hiding.
 * This function is the onmouseover event handler for context menu.
 *
 */
o2jse.cMenu.mouseOver = function() {

    clearTimeout(o2jse.cMenu.timeout);

    };


/**
 * Starts timeout running for menu hiding.
 * This function is the onmouseout event handler for context menu.
 *
 */
o2jse.cMenu.mouseOut = function() {

    o2jse.cMenu.timeout = setTimeout(o2jse.cMenu.setOff, 1000);

    };


/**
 * Load active items in menu context
 *
 * @returns boolean
 * @type    boolean
 */
o2jse.cMenu.loadItems = function() {

    var somethingIn = false;
    // _________________________________________________________________ Clear context ___
    o2jse.cMenu.clear();
    if (o2jse.cMenu.partnersB.length > 0) {
        // ________________________________________________ Items from partners BEFORE ___
        for (var partnerId = o2jse.cMenu.partnersB.length; partnerId > 0; partnerId--) {
            somethingIn = (o2jse.cMenu.partnersB[partnerId - 1]() || somethingIn);
            }
        }
    // ________________________________________________ Add data-history to admin menu ___
    o2jse.ctrl.init(o2jse.cMenu.target);
    if (o2jse.cMenu.target.o2.log) {
        o2jse.cMenu.addItem("J",
                            "jxdatalog",
                            "Data history",
                            function() { o2jse.ctrl.log(o2jse.cMenu.target); },
                            o2jse.rntAlias + "img/history.png");
        somethingIn = true;
        }
    // ______________________________________________________________ Application menu ___
    if (o2jse.contMenuApp) {
        appSubMenu = false;
        if (o2jse.menu.menuList[o2jse.menu.appMainMenu]) {
            appSubMenu = true;
            }
        else if (o2jse.menu.appMenu) {
            o2jse.menu.appMenu();
            appSubMenu = true;
            }
        if (appSubMenu) {
            o2jse.cMenu.addItem("M",
                                o2jse.menu.appMainMenu,
                                "App menu",
                                "",
                                o2jse.rntAlias + "img/appmenu.gif");
            somethingIn = true;
            }
        }
    // ___________________________________________________________ Administrator tools ___
    if (o2jse.superUser) {
        o2jse.cMenu.addAdmin();
        somethingIn = true;
        }
    // _________________________________________________________ Open new session menu ___
    if (o2jse.contMenuNewSess) {
        o2jse.cMenu.addItem("J",
                            "jxnewsess",
                            "Open new session",
                            function() {
                                o2jse.requester.exe("sessopen",
                                                    "",
                                                    o2jse.cMenu,
                                                    jxjs.jsEval);
                                return false;
                                },
                            o2jse.rntAlias + "img/sess_new.png");
        somethingIn = true;
        }
    if (o2jse.cMenu.partnersA.length > 0) {
        // _________________________________________________ Items from partners AFTER ___
        for (var partnerId = o2jse.cMenu.partnersA.length; partnerId > 0; partnerId--) {
            somethingIn = (o2jse.cMenu.partnersA[partnerId - 1]() || somethingIn);
            }
        }
    return somethingIn;

    };


/**
 * Load administrative tools in menu context
 *
 */
o2jse.cMenu.addAdmin = function() {

    o2jse.cMenu.addItem("M",
                        "o2admin",
                        "Administrator",
                        "",
                        o2jse.rntAlias + "img/admin.png");
    o2jse.menu.menuList["o2admin"].clear();
    if (o2jse.menu.menuList["o2options"]) {
        o2jse.menu.menuList["o2options"].clear();
        }
    if (o2jse.menu.menuList["jxdoc_folder"]) {
        o2jse.menu.menuList["jxdoc_folder"].clear();
        }
    if (o2jse.menu.menuList["jxmultilang"]) {
        o2jse.menu.menuList["jxmultilang"].clear();
        }
    o2jse.menu.admMenu();
    o2jse.ctrl.init(o2jse.cMenu.target);
    if (o2jse.doc) {
        // _____________________________________________________ Add JXDocumentor menu ___
        o2jse.menu.menuList["o2admin"].addItem("M",
                                               "jxdoc_folder",
                                               "Documentation",
                                               "",
                                               o2jse.rntAlias + "img/jxdoc.png");
        var pMenu = o2jse.menu.menuList["jxdoc_folder"];
        var done  = false;
        // __________________________ Add "Documentation by menu" to JXDocumentor menu ___
        if (o2jse.cMenu.target.o2.cT == "menuitem") {
            var func2exe = function() {
                              o2jse.cmd.run("doc/jxdoc_start",
                                            0,
                                            o2jse.cMenu.target.o2.prgName);
                              };
            pMenu.addItem("J",
                          "jxdocbymenu",
                          "Document menu item <b>" + o2jse.cMenu.target.o2.itemLabel +
                                                                                   "</b>",
                          func2exe,
                          o2jse.rntAlias + "img/jxdoc_bymenu.png");
            done = true;
            }
        // _________________ Add "Documentation by ctrl/form/prg" to JXDocumentor menu ___
        if (o2jse.cMenu.target.o2.f) {
            pMenu.addItem("J",
                          "jxdocbyctrl",
                          "Document this control, form or program",
                          function() { o2jse.cmd.doc(o2jse.cMenu.target); },
                          o2jse.rntAlias + "img/jxdoc.png");
            done = true;
            }
        if (done) {
            pMenu.addItem("S", "jxdocsep");
            }
        // __________________________________________________ Add  JXDocumentor "uses" ___
        pMenu.addItem("P",
                      "jxdoc_uses",
                      "Uses",
                      "doc/uses_select",
                      o2jse.rntAlias + "img/jxdoc_tool.png");
        // ______________________________________________ Add  JXDocumentor "settings" ___
        pMenu.addItem("P",
                      "jxdoc_settings",
                      "Settings",
                      "doc/settings_admin",
                      o2jse.rntAlias + "img/jxdoc_tool.png");
        }
    // ____________________________________________ Add "menu profiling" to admin menu ___
    if (o2jse.profiling) {
        var pMenu = o2jse.menu.menuList["o2admin"];
        switch (o2jse.cMenu.target.o2.cT) {
            case "menuitem":
                if (o2jse.profiling == "M" || o2jse.profiling == "B") {
                    var func2exe = function() {
                                      o2jse.cmd.run("tools/o2sys_profile_menu",
                                                    o2jse.cMenu.target.o2.c);
                                      };
                    pMenu.addItem("J",
                                  "o2profilemenu",
                                  "Profile menu item <b>" +
                                  o2jse.cMenu.target.o2.itemLabel + "</b>",
                                  func2exe,
                                  o2jse.rntAlias + "img/profiling.gif");
                    }
                break;
            case "button":
            case "edit":
            case "listcombo":
            case "checkbox":
            case "text":
            case "file":
            case "img":
                if (o2jse.profiling == "C" || o2jse.profiling == "B") {
                    var func2exe = function() {
                                      o2jse.cmd.run("tools/o2sys_profile_ctrl",
                                                    o2jse.infoForm['o2_prgname'].value,
                                                    o2jse.cMenu.target.o2.c);
                                      };
                    pMenu.addItem("J",
                                  "o2profilectrl",
                                  "Profile <b>"+ o2jse.cMenu.target.o2.cT+ "</b> control",
                                  func2exe,
                                  o2jse.rntAlias + "img/profiling.gif");
                    }
                break;
            }
        }

    };


/**
 * Register a module as "partner" for adding items to context menu. Module will be also
 * responsable for items selection.
 *
 * Module registration
 * ===================
 * For registering a module must provide a function: this function will be called on menu
 * initialization and will be used to add items to context menu.
 *
 * NOTE: o2jse.cMenu.target object can be tested for differencing contextual behaviors.
 *
 *    function funcInit() = { [see o2jse.cMenu.addItem() method] };
 *    o2jse.cMenu.regPartner(funcInit);
 *
 * @param {Function} funcObj   Function passed as initializer
 * @param {Boolean}  before    If passed TRUE partner elements are added before others
 */
o2jse.cMenu.regPartner = function(funcObj, before) {

    if (before) {
        o2jse.cMenu.partnersB[o2jse.cMenu.partnersB.length] = funcObj;
        }
    else {
        o2jse.cMenu.partnersA[o2jse.cMenu.partnersA.length] = funcObj;
        }

    };


/**
 * Prototype for Janox calendar object
 *
 * @param {Object}  pD       Initial date to show
 * @param {Object}  retObj   Field object to update with selected date
 * @param {Integer} posX     Value of LEFT for positioning
 * @param {Integer} posY     Value of TOP for positioning
 * @param {Boolean} framed   If calendar has to be created in its own frame box (DIV)
 */
function jxCalendar(pD, retObj, posX, posY, framed) {

    this.mList  = new Array('January', 'February', 'March', 'April', 'May', 'June',
                            'July', 'August', 'September', 'October', 'November',
                            'December');
    this.dList  = new Array('Mon', 'Tue', 'Wed', 'Thu', 'Fri ', 'Sat', 'Sun');
    this.eList  = new Array(31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31);
    this.today  = new Date();
    this.xMonth = 0;
    this.retObj = (typeof retObj == "object" ? retObj : false);
    // _______________________________________________________________________ Out box ___
    if (framed) {
        this.outBox                = document.createElement("DIV");
        this.outBox.className      = "caleBox";
        this.outBox.style.position = "absolute";
        this.outBox.style.left     = posX + "px";
        this.outBox.style.top      = posY + "px";
        }
    // _________________________________________________________________ Selected date ___
    if (pD) {
        this.year  = pD.getFullYear();
        this.month = pD.getMonth();
        this.day   = pD.getDate();
        }
    else {
        this.year  = this.today.getFullYear();
        this.month = this.today.getMonth();
        this.day   = this.today.getDate();
        }


    /**
     * Ends calendar functionalities. Field is updated with value (if retObj is defined)
     * and calendar is closed.
     *
     * @param integer pD   Day of date to set
     * @param integer pM   Month of date to set
     * @param integer pY   Year of date to set
     */
    this.action = function(pD, pM, pY) {

       if (this.retObj) {
            this.retObj.value = o2jse.cale.dateFormat(pD, pM, pY, this.retObj.o2.m);
            if (o2jse.cliMode && this.retObj.o2.fret) {
                jxjs.request(this.retObj, this.retObj.value);
                }
            else {
                o2jse.cmd.ctrlUpd(this.retObj);
                }
            }
        this.close();

        };


    /**
     * Closes calendar and (if one) related context menu
     *
     */
    this.close = function() {

        o2jse.removeEl(o2jse.ctrl.caleObj.outBox);
        delete o2jse.ctrl.caleObj;
        o2jse.menu.closeAll();

        };


    /**
     * Compares two date objects and returns TRUE if they are the same date
     *
     * @param object d1   First of two dates to compare
     * @param object d2   Second of two dates to compare
     */
    this.comparedate = function(d1, d2) {

        return d1.getFullYear() == d2.getFullYear() &&
               d1.getMonth()    == d2.getMonth()    &&
               d1.getDate()     == d2.getDate();

        };


    this.pasqua = function(aa) {

        var xx = new Array(22, 22, 23, 23, 24, 24);
        var yy = new Array(2, 2, 3, 4, 5, 5);
        var a  = aa % 19;
        var b  = aa % 4;
        var c  = aa % 7;
        var i  = Math.floor(aa / 100) - 15;
        var x  = xx[i], y = yy[i];
        var d  = (19 * a + x) % 30;
        var e  = (2 * b + 4 * c + 6 * d + y) % 7;
        var p  = 22 + d + e;
        var m  = 300;
        if (p > 31) {
            m = 400;
            p = p - 31;
            }
        return m + p;

        };


    this.isRed = function(pD) {

        var reds = new Array(0, 101, 106, 501, 815, 1101, 1225, 1226);

        if (pD.getDay() == 0) {
            return true;
            }
        var pp = this.pasqua(pD.getFullYear()) + 1;
        // Pasquetta
        if (pp % 100 > 31) {
            pp = 401;
            }
        reds[0] = pp;
        for (var i = 0; i < reds.length; i++) {
            if (Math.floor(reds[i] / 100) == pD.getMonth() + 1) {
                var gg = reds[i] % 100;
                if (gg == pD.getDate()) {
                    return true;
                    }
                }
            }
        return false;

        };


    this.calendar = function(currday, pMonth, pYear) {

        var dx = 2 - (new Date(pYear, pMonth, 1)).getDay();
        if (dx == 2) {
            dx = -5;
            }
        var daysInMonth = this.eList[pMonth];
        // Bisestile
        if (pMonth == 1) {
            if (pYear % 4 == 0 && pYear % 100 != 0 || pYear % 400 == 0) {
                daysInMonth++;
                }
            }
        // _______________________________________________________________ Whole frame ___
        var frameTable          = document.createElement("TABLE");
        frameTable.width        = "100%";
        frameTable.border       = "0";
        frameTable.cellPadding  = "1px";
        frameTable.cellSpacing  = "1px";
        // _____________________________________________________________________ Title ___
        var rowTitle            = frameTable.insertRow(-1);
        // _____________________________________________________ Decrease month button ___
        var mLessBtn            = rowTitle.insertCell(-1);
        mLessBtn.innerHTML      = "\u00ab";
        mLessBtn.className      = "caleDmBtn";
        mLessBtn.onclick        = function() { o2jse.ctrl.caleObj.changeMonth(-1); };
        // ______________________________________________________________ Month & year ___
        var cellTitle           = rowTitle.insertCell(-1);
        cellTitle.colSpan       = "5";
        cellTitle.className     = "caleTitle";
        cellTitle.innerHTML     = this.mList[pMonth] + ' ' + pYear;
        // _____________________________________________________ Increase month button ___
        var mPlusBtn            = rowTitle.insertCell(-1);
        mPlusBtn.innerHTML      = "\u00bb";
        mPlusBtn.className      = "caleImBtn";
        mPlusBtn.onclick        = function() { o2jse.ctrl.caleObj.changeMonth(1); };
        // ___________________________________________________________ Week day labels ___
        var rowWDlabels         = frameTable.insertRow(-1);
        for (var i = 0; i < 7; i++) {
            var cellWDLabel       = rowWDlabels.insertCell(-1);
            cellWDLabel.className = "caleLabel";
            cellWDLabel.innerHTML = this.dList[i];
            }
        // __________________________________________________________ Rows in calendar ___
        for (var j = 0; j < 6; j++) {
            var rowInCale = frameTable.insertRow(-1);
            rowInCale.style.textAlign = "right";
            for (var i = 0; i < 7; i++) {
                var cellDay                   = rowInCale.insertCell(-1);
                cellDay.className             = "cal_label";
                cellDay.style.backgroundColor = "#F5F5F5";
                cellDay.innerHTML             = " &nbsp; ";
                if (dx > 0 && dx <= daysInMonth) {
                    var ddx = new Date(pYear, pMonth, dx);
                    if (this.isRed(ddx)) {
                        cellDay.style.backgroundColor = "#FF9900";
                        }
                    var tempdate = ddx.getDate() + ", " +
                                   ddx.getMonth() + ", " +
                                   ddx.getFullYear();
                    cellDay.innerHTML = '<a href="javascript:o2jse.ctrl.caleObj.action(' +
                                        tempdate + ')" class="caleLabel" ' +
                                        'style="text-decoration:none">' +
                                        (this.comparedate(ddx, this.today) ?
                                         '<b><u>' + dx + '</u></b>' :
                                         dx) + '</a>';
                    }
                dx++;
                }
            }
        return frameTable;

        };


    this.dataFrame = function(caleFrame) {

        // _______________________________________________________________ Whole frame ___
        var frameTable;
        if (this.outBox) {
            this.outBox.removeChild(this.outBox.firstChild);
            if (this.outBox.tagName == "DIV") {
                frameTable = o2jse.createEl(this.outBox, "TABLE");
                }
            else {
                frameTable = this.outBox;
                }
            }
        else {
            this.outBox = document.createElement("TABLE");
            frameTable  = this.outBox;
            }
        frameTable.className     = "caleFrame";
        var rowCale              = frameTable.insertRow(-1);
        // __________________________________________________________________ Calendar ___
        var cellCale             = rowCale.insertCell(-1);
        var rowCmds              = frameTable.insertRow(-1);
        var cellCmds             = rowCmds.insertCell(-1);
        cellCmds.style.textAlign = "right";
        cellCale.appendChild(caleFrame);
        // ________________________________________________________ Go to today button ___
        var todayBtn             = o2jse.createInput(cellCmds, "button", "caleTodayBtn");
        todayBtn.onclick         = function() { o2jse.ctrl.caleObj.changeMonth(9); };
        // ______________________________________________________________ Close button ___
        var closeBtn             = o2jse.createInput(cellCmds, "button", "caleCloseBtn");
        closeBtn.onclick         = this.close;

        };


    this.changeMonth = function(x) {

        if (x == 9) {
            this.xMonth = 0;
            }
        else {
            this.xMonth += x;
            }
        var xday = (this.xMonth == 0) ? this.day : 99;
        var mm   = (this.month + this.xMonth) % 12;
        while (mm < 0) {
            mm += 12;
            }
        this.dataFrame(this.calendar(xday,
                                     mm,
                                     this.year +
                                     Math.floor((this.month + this.xMonth) / 12)));

        };


    this.changeMonth(this.xMonth);

    };


/**
 * Calendar module
 */
o2jse.cale = {};


o2jse.cale.dateFormat = function(pD, pM, pY, maskStr) {

    function lFormat(x) { return x > 9 ? String(x) : "0" + x; };

    maskStr      = maskStr.toLowerCase();
    var rawValue = "";
    var shortY   = maskStr.indexOf("2") > -1;
    if (shortY) {
        pY = String(pY).substr(-2);
        }
    if (maskStr.indexOf("d") == -1 &&
        maskStr.indexOf("m") == -1 &&
        maskStr.indexOf("y") == -1) {
        rawValue = lFormat(pD) + lFormat(pM + 1) + pY;
        }
    else {
        maskStr  = maskStr.replace("2", "");
        maskStr  = maskStr.replace("z", "");
        rawValue = maskStr.replace("d", lFormat(pD)).
                      replace("m", lFormat(pM + 1)).replace("y", lFormat(pY));
        }
    return o2jse.data.fd(rawValue, maskStr);

    };


o2jse.cale.setToday = function() {

    var targetObj = o2jse.cMenu.target;
    o2jse.ctrl.init(targetObj);
    var pD          = new Date();
    targetObj.value = o2jse.cale.dateFormat(pD.getDate(),
                                            pD.getMonth(),
                                            pD.getFullYear(),
                                            targetObj.o2.m);
    if (o2jse.cliMode && targetObj.o2.fret) {
        jxjs.request(targetObj, targetObj.value);
        }
    else {
        o2jse.cmd.ctrlUpd(targetObj);
        }
    o2jse.menu.closeAll();

    };


o2jse.cale.initContMenu = function() {

    if (o2jse.cMenu.target.o2.dT == "D") {
        var pD = new Date();
        var sD = o2jse.cale.dateFormat(pD.getDate(),
                                       pD.getMonth(),
                                       pD.getFullYear(),
                                       o2jse.cMenu.target.o2.m);
        o2jse.cMenu.addItem("J",
                            "o2caleSetToday",
                            "<b>" + sD + "</b>",
                            o2jse.cale.setToday,
                            o2jse.rntAlias + "img/left.png");
        o2jse.cMenu.addItem("M",
                            "o2caleShow",
                            "Calendar",
                            "",
                            o2jse.rntAlias + "img/cale.gif");
        o2jse.ctrl.caleObj = new jxCalendar(pD, o2jse.cMenu.target, 0, 0, false);
        o2jse.menu.menuList["o2caleShow"].context(o2jse.ctrl.caleObj.outBox);
        return true;
        }
    else {
        return false;
        }

    };


o2jse.docInitContMenu = function() {

    if (document.forms.o2form['o2_prgexeid'].value > 0) {
        o2jse.cMenu.addItem("J",
                            "o2Help",
                            "Documentation",
                            function() {
                                o2jse.cmd.doc(o2jse.cMenu.target);
                                return false;
                                },
                            o2jse.rntAlias + "img/jxdoc.png");
        return true;
        }
    else {
        return false;
        }

    };
