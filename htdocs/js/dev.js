/****************************************************************************************
 *   file: jxrnt/htdocs/js/dev.js                                                       *
 *   desc: Javascript development tools                                                 *
 *   copy: www.janox.it, 2007                                                           *
 *   lang: javascript                                                                   *
 *   vers: 5                                                                            *
 ****************************************************************************************/


/**
 * Alias for o2je.dev.show()
 *
 * @param object  local_obj   Object to show
 * @param integer deepLimit   Maximum nesting level descending
 */
function o2log(local_obj, deepLimit) {

    return o2jse.dev.show(local_obj, deepLimit);

    }


/**
 * Development utility functions collection
 *
 */
o2jse.dev = {

    timeAnalysis : false,         /* If is active time analysis procedure               */
    rights       : {},            /* List of defined rights                             */
    roles        : {}             /* List of defined roles                              */

    };


/**
 * Prints out object structure
 *
 * @param object  local_obj   Object to show
 * @param integer deepLimit   Maximum nesting level descending
 * @param integer indent      Current level indentation
 * @param string  propName    Current showing property
 */
o2jse.dev.show = function(local_obj, deepLimit, indent, propName) {

    o2jse.elBody = (document.body || document.getElementsByTagName("body")[0]);
    if (o2jse.dText == null ||
        o2jse.dText == ""   ||
        deepLimit == null   ||
        deepLimit == 0) {
        o2jse.dText = "";
        }
    deepLimit      = (deepLimit > 0 ? deepLimit : 0);
    indent         = (indent > 0 ? indent : 0);
    var spInd      = "";
    var spIndInt   = "";
    var prop_local = null;
    if (typeof propName == "undefined") {
        propName = "";
        }
    for (var i = 0; i < indent; i++) {
        spInd += " &nbsp; ";
        }
    if (typeof local_obj == "object") {
        o2jse.dText += spInd +  "<b>" + (propName != "" ? propName + " " : "") +
                       ("(" + (typeof local_obj) + ")</b>: {<br />");
        spIndInt      = spInd + " &nbsp; &nbsp; &nbsp; ";
        for (var prop in local_obj) {
            prop_local = local_obj[prop];
            if (typeof prop_local == "object" && indent < deepLimit) {
                o2jse.dev.show(prop_local, deepLimit, (indent + 1), prop);
                }
            else {
                o2jse.dText += spIndInt + ("<b>" + prop + " (" + (typeof prop_local) +
                                           ")</b>: " + prop_local + "<br />");
                }
            }
        o2jse.dText += spInd + "}<br />";
        }
    else {
        o2jse.dText += spInd + ("<b>(" + (typeof prop_local) + ")</b>: " + local_obj +
                                 "<br />");
        }
    if (indent < 1) {
        o2jse.dText+= "<br /><br /><br />";
        o2jse.lab.getResult(null, o2jse.dText);
        o2jse.dText = false;
        }

    };


/**
 * Open form for reporting a bug
 *
 * @return void
 */
o2jse.dev.bugReport = function() {

    var pageLoc = location.pathname;
    var appName = pageLoc.slice(pageLoc.lastIndexOf("/") + 1, -4);
    var exeList = "";
    var exePrg  = "";
    for (var singleExe in o2jse.exeTree) {
        var exeObj = o2jse.exeTree[singleExe];
        if (exeObj.type == "P") {
            exePrg = exeObj.name;
            exeList+= "[" + exeObj.prg + "] " + exeObj.name + "\n";
            }
        else {
            exeList+= " |- " + exeObj.act + " (" + exeObj.step + ")\n";
            }
        }
    var mSubject        = appName.toUpperCase() + "%20[" + exePrg + "]%20%40" +
                          o2jse.devName;
    var mForm           = o2jse.createEl(document.body, "FORM");
    mForm.style.display = "none";
    mForm.method        = "POST";
    mForm.action        = "mailto:support@janox.it?subject=" + mSubject;
    mForm.enctype       = "text/plain";
    var mList           = o2jse.createEl(mForm,
                                         "TEXTAREA",
                                         null,
                                         "=========\n\n" + exeList);
    mList.name          = "Execution-list";
    mForm.submit();
    o2jse.removeEl(mForm);

    };


/**
 * Load development tools in menu context
 *
 */
o2jse.dev.addDevMenu = function() {

    // _____________________________________________________________________ Separator ___
    o2jse.cMenu.addItem("S");
    // _____________________________________ Add development menu item to context menu ___
    o2jse.cMenu.addItem("M",
                        "o2cmenu_dev",
                        o2jse.devName,
                        "",
                        o2jse.rntAlias +"img/jxdev.png");
    var devMenu = o2jse.menu.menuList["o2cmenu_dev"];
    devMenu.clear();
    // __________________________________________________ Add session info to dev menu ___
    o2jse.dev.cMenuDevSessionInfo();
    // __________________________________________________ Add request data to dev menu ___
    o2jse.dev.cMenuDevReqData();
    // __________________________________________ Add executions tree item to dev menu ___
    o2jse.dev.cMenuDevExeTree();
    // __________________________________________________ Add UI-Info item to dev menu ___
    o2jse.dev.cMenuDevUIInfo();
    // ___________________________________________________ Add rights list to dev menu ___
    o2jse.dev.cMenuDevRights();
    // ____________________________________________________ Add roles list to dev menu ___
    o2jse.dev.cMenuDevRoles();
    // _____________________________________________________ Add dev tools to dev menu ___
    o2jse.dev.cMenuDevTools();
    // _____________________________________________________ Add Janox Support request ___
    /*
    devMenu.addItem("J", "bug_report", "Report a bug", o2jse.dev.bugReport,
                    o2jse.rntAlias +"img/jxbug.png");
    */

    };


/**
 * Returns User Interface Informations panel for development context menu
 *
 */
o2jse.dev.cMenuDevUIInfo = function() {

    o2jse.menu.menuList["o2cmenu_dev"].addItem("M",
                                               "o2uiinfo",
                                               "Interface info",
                                               "",
                                               o2jse.rntAlias +"img/sess_new.png");
    var frameTable       = document.createElement("TABLE");
    frameTable.className = "o2devInfo";
    var titlesRow        = frameTable.createTHead().insertRow(-1);
    o2jse.createEl(titlesRow, "TH", "", "<b>Type</b>");
    o2jse.createEl(titlesRow, "TH", "", "<b>Property</b>");
    o2jse.createEl(titlesRow, "TH", "", "<b>Value</b>");
    o2jse.ctrl.init(o2jse.cMenu.target);
    const props = {
        e: 'Execution index',
        c: 'Control name',
        cT: 'Control type',
        v: 'View',
        fld: 'Field',
        f: 'Form name',
        pT: 'Parent type',
        dT: 'Data type',
        m: 'Data mask',
        s: 'Size',
        log: 'Log active',
        fret: 'Submit on change',
        nav: 'Related navigator',
        cssc: 'CSS class',
        msg: 'Message',
        z: 'Zoom',
        puact: 'Popup action',
        puexp: 'Popup expression',
        vS: 'Submit on click',
        inctrl: 'Column control',
        col: 'Column index',
        rows: 'Rows per page',
        action: 'Action',
        page: 'Page index',
        alignH: 'Horizontal align',
        alignV: 'Vertical align',
        x: 'X',
        y: 'Y',
        exit: 'Allow close',
        dyn: 'Dynamic list',
        cssf: 'Field CSS class',
        cssl: 'List CSS class',
        subf: 'Number of sub-forms',
        rt: '(Not used)'
    };
    for (var sProp in o2jse.cMenu.target.o2) {
        var singlePropRow   = frameTable.insertRow(-1);
        var typeCell        = singlePropRow.insertCell(-1);
        var propCell        = singlePropRow.insertCell(-1);
        var valueCell       = singlePropRow.insertCell(-1);
        typeCell.innerHTML  = "<i>" + (typeof o2jse.cMenu.target.o2[sProp]) + "</i>";
        propCell.innerHTML  = "<b>" + (sProp in props ? props[sProp] : sProp) + "</b>";
        valueCell.innerHTML = "<code>" + o2jse.cMenu.target.o2[sProp] + "</code>";
        }
    o2jse.menu.menuList["o2uiinfo"].context(frameTable);

    };


/**
 * Returns Execution Tree panel for development context menu
 *
 */
o2jse.dev.cMenuDevExeTree = function() {

    o2jse.menu.menuList["o2cmenu_dev"].addItem("M",
                                               "o2exeTree",
                                               "Exe tree",
                                               "",
                                               o2jse.rntAlias +"img/exetree.png");
    o2jse.menu.menuList["o2exeTree"].clear();
    o2jse.menu.menuList["o2exeTree"].addItem('R',
                                             'jxExeTreeTitle1',
                                             '<td>&nbsp;</td><td>&nbsp;<b>#</b>&nbsp;' +
                                             '&nbsp;&nbsp;<b>Program</b></td><td>&nbsp;' +
                                             '</td><td>&nbsp;</td>');
    o2jse.menu.menuList["o2exeTree"].addItem('R',
                                             'jxExeTreeTitle2',
                                             '<td>&nbsp;</td><td>&nbsp;&nbsp;&nbsp;' +
                                             '&nbsp;&nbsp;<b>Action</b></td><td>' +
                                             '<b>Step</b></td><td>&nbsp;</td>');
    o2jse.menu.menuList["o2exeTree"].addItem("S");
    for (var singleExe in o2jse.exeTree) {
        var exeObj        = o2jse.exeTree[singleExe];
        if (exeObj.type == "P") {
            if (exeObj.pars.length > 0) {
                o2jse.menu.menuList["o2exeTree"].addItem("M",
                                                         exeObj.name,
                                                         exeObj.prg +
                                                         '&nbsp;&nbsp;&nbsp;<b>' +
                                                         exeObj.name + '</b>');
                o2jse.menu.menuList[exeObj.name].clear();
                o2jse.menu.menuList[exeObj.name].addItem('R',
                                                         exeObj.name + 'PTitle',
                                                         '<td colspan="3"><center><b>' +
                                                         'Parameters</b></center></td>');
                o2jse.menu.menuList[exeObj.name].addItem('S');
                var parsCode = '';
                var parCode  = '';
                for (var par in exeObj.pars) {
                    parTxt  = exeObj.pars[par];
                    parCode = '<td style="padding-left:10px;padding-right:10px;">' +
                              parTxt.replace(/\s/g,
                                             '</td><td style="padding-left:10px' +
                                             ';padding-right:10px;">') +
                              '</td>';
                    o2jse.menu.menuList[exeObj.name].addItem('R',
                                                             exeObj.name + 'P' + par,
                                                             parCode);
                    }
                o2jse.menu.menuList[exeObj.name].addItem('S');
                o2jse.menu.menuList[exeObj.name].addItem('R',
                                                         exeObj.name + 'P' + par,
                                                         '<td colspan="3"><center>' +
                                                         '<input type="button" ' +
                                                         'value="Execute from here" ' +
                                                         'onclick="o2jse.dev.' +
                                                         'reExeModule(' + exeObj.prg +
                                                         ');"></center></td>');
                }
            else {
                o2jse.menu.menuList["o2exeTree"].addItem("R",
                                                         exeObj.name,
                                                         '<td>&nbsp;</td>' +
                                                         '<td class="o2menuLabel">' +
                                                         exeObj.prg +
                                                         '&nbsp;&nbsp;&nbsp;<b>' +
                                                         exeObj.name + '</b></td>');
                }
            }
        else {
            o2jse.menu.menuList["o2exeTree"].addItem("R",
                                                     exeObj.act + exeObj.step,
                                                     '<td>&nbsp;</td><td>&nbsp;&nbsp;' +
                                                     '&nbsp;&nbsp;&nbsp;' + exeObj.act +
                                                     '</td><td style="text-align:right;">'
                                                   + exeObj.step + '</td>');
            }
        }

    };


/**
 * Returns Request Data panel for development context menu
 *
 */
o2jse.dev.cMenuDevReqData = function() {

    o2jse.menu.menuList["o2cmenu_dev"].addItem("M",
                                               "o2reqData",
                                               "Request data",
                                               "",
                                               o2jse.rntAlias +"img/reqdata.png");
    var frameTable       = document.createElement("TABLE");
    frameTable.className = "o2devInfo";
    var titlesRow        = frameTable.createTHead().insertRow(-1);
    o2jse.createEl(titlesRow, "TH", "", "<b>Field</b>");
    o2jse.createEl(titlesRow, "TH", "", "<b>Value</b>");
    for (var singleField in o2jse.reqData) {
        var singleRow       = frameTable.insertRow(-1);
        var fieldCell       = singleRow.insertCell(-1);
        var valueCell       = singleRow.insertCell(-1);
        var fieldValue      = o2jse.reqData[singleField];
        fieldCell.innerHTML = "<b>" + singleField + "</b>";
        valueCell.innerHTML = "<code>" + fieldValue + "</code>";
        }
    o2jse.menu.menuList["o2reqData"].context(frameTable);

    };


/**
 * Returns Development Tools panel for development context menu
 *
 */
o2jse.dev.cMenuDevTools = function() {

    if (o2jse.menu.menuList["o2devTools"]) {
        o2jse.menu.menuList["o2devTools"].clear();
        }
    // _______________________________________________________ Add DEV-TOOLS menu item ___
    o2jse.menu.menuList["o2cmenu_dev"].addItem("M",
                                               "o2devTools",
                                               "Dev tools",
                                               "",
                                               o2jse.rntAlias +"img/jxtools.png");
    var t = o2jse.menu.menuList["o2devTools"];
    // ________________________________________________________ Execute module by name ___
    t.addItem("P", "prg_by_name", "Execute module", "tools/o2dev_runner",
              o2jse.rntAlias + "img/run.png");
    // __________________________________________________________ Application analysis ___
    t.addItem("P", "app_inspector", "Inspector", "tools/jxinspector",
              o2jse.rntAlias + "img/inspector.png");
    // __________________________________________________________ Application analysis ___
    t.addItem("P", "app_analysis", "Application analysis", "tools/o2dev_app_analysis",
              o2jse.rntAlias + "img/analysis.png");
    // ________________________________________________________ Split table repository ___
    t.addItem("P", "split_repos", "Split table repository", "tools/o2dev_split_rep",
              o2jse.rntAlias + "img/split.png");
    // ___________________________________________________________ Generate login page ___
    t.addItem("P", "login_page", "Generate login page", "tools/o2dev_login_page",
              o2jse.rntAlias + "img/profiling.gif");
    // __________________________________________________________ Application analysis ___
    t.addItem("P", "app_obscure", "Obscure application", "tools/jxobscure",
              o2jse.rntAlias + "img/jxlock.png");

    };


/**
 * Returns Rights list panel for development context menu
 *
 */
o2jse.dev.cMenuDevRights = function() {

    o2jse.menu.menuList["o2cmenu_dev"].addItem("M",
                                               "o2rights",
                                               "Rights",
                                               "",
                                               o2jse.rntAlias +"img/rights.gif");
    var frameTable       = document.createElement("TABLE");
    frameTable.className = "o2devInfo";
    var titlesRow        = frameTable.createTHead().insertRow(-1);
    o2jse.createEl(titlesRow, "TH", "", "<b>Right</b>");
    for (var singleRight in o2jse.dev.rights) {
        var singleRow       = frameTable.insertRow(-1);
        var rightCell       = singleRow.insertCell(-1);
        rightCell.innerHTML = o2jse.dev.rights[singleRight];
        }
    o2jse.menu.menuList["o2rights"].context(frameTable);

    };


/**
 * Returns Roles list panel for development context menu
 *
 */
o2jse.dev.cMenuDevRoles = function() {

    o2jse.menu.menuList["o2cmenu_dev"].addItem("M",
                                               "o2roles",
                                               "Roles",
                                               "",
                                               o2jse.rntAlias +"img/roles.gif");
    var frameTable       = document.createElement("TABLE");
    frameTable.className = "o2devInfo";
    var titlesRow        = frameTable.createTHead().insertRow(-1);
    o2jse.createEl(titlesRow, "TH", "", "<b>Role</b>");
    for (var singleRole in o2jse.dev.roles) {
        var singleRow      = frameTable.insertRow(-1);
        var roleCell       = singleRow.insertCell(-1);
        roleCell.innerHTML = o2jse.dev.roles[singleRole];
        }
    o2jse.menu.menuList["o2roles"].context(frameTable);

    };


/**
 * Returns Roles list panel for development context menu
 *
 */
o2jse.dev.cMenuDevSessionInfo = function() {

    o2jse.menu.menuList["o2cmenu_dev"].addItem("M",
                                               "o2session",
                                               "Session",
                                               "",
                                               o2jse.rntAlias +"img/session.gif");
    var frameTable       = document.createElement("TABLE");
    frameTable.className = "o2devInfo";
    var titlesRow        = frameTable.createTHead().insertRow(-1);
    o2jse.createEl(titlesRow, "TH", "", "<b>Property</b>");
    o2jse.createEl(titlesRow, "TH", "", "<b>Value</b>");
    // _______________________________________________________________________ Runtime ___
    var singleRow       = frameTable.insertRow(-1);
    var propCell        = singleRow.insertCell(-1);
    var valueCell       = singleRow.insertCell(-1);
    propCell.innerHTML  = "<b>Runtime</b>";
    valueCell.innerHTML = "<code>" + o2jse.rntAlias + "</code>";
    // ____________________________________________________________________ Applicaion ___
    singleRow           = frameTable.insertRow(-1);
    propCell            = singleRow.insertCell(-1);
    valueCell           = singleRow.insertCell(-1);
    propCell.innerHTML  = "<b>Application</b>";
    valueCell.innerHTML = "<code>" + o2jse.appAlias + "</code>";
    // ______________________________________________________________ PHP session name ___
    singleRow           = frameTable.insertRow(-1);
    propCell            = singleRow.insertCell(-1);
    valueCell           = singleRow.insertCell(-1);
    propCell.innerHTML  = "<b>Session name</b>";
    valueCell.innerHTML = "<code>" + o2jse.sessName + "</code>";
    // ________________________________________________________________ PHP session id ___
    singleRow           = frameTable.insertRow(-1);
    propCell            = singleRow.insertCell(-1);
    valueCell           = singleRow.insertCell(-1);
    propCell.innerHTML  = "<b>Session ID</b>";
    valueCell.innerHTML = (o2jse.infoForm[o2jse.sessName] ?
                           "<code>" + o2jse.infoForm[o2jse.sessName].value + "</code>" :
                           "<i>As cookie</i>");
    // __________________________________________________________________________ User ___
    singleRow           = frameTable.insertRow(-1);
    propCell            = singleRow.insertCell(-1);
    valueCell           = singleRow.insertCell(-1);
    propCell.innerHTML  = "<b>User</b>";
    valueCell.innerHTML = "<code>" + o2jse.user + "</code>";
    // ______________________________________________________________________ Pagemark ___
    singleRow           = frameTable.insertRow(-1);
    propCell            = singleRow.insertCell(-1);
    valueCell           = singleRow.insertCell(-1);
    propCell.innerHTML  = "<b>Pagemark</b>";
    valueCell.innerHTML = "<code>" + o2jse.infoForm['o2pagemark'].value + "</code>";
    // _____________________________________________________________________ Developer ___
    singleRow           = frameTable.insertRow(-1);
    propCell            = singleRow.insertCell(-1);
    valueCell           = singleRow.insertCell(-1);
    propCell.innerHTML  = "<b>Developer</b>";
    valueCell.innerHTML = "<code>" + o2jse.devName + "<code>";
    o2jse.menu.menuList["o2session"].context(frameTable);

    };


/**
 * Start/stop mute console log  - Log only to file
 *
 */
o2jse.dev.reExeModule = function(exeId) {

    o2jse.requester.exe("jxdev",
                        "jxexemodule=" + exeId,
                        o2jse.cMenu,
                        jxjs.jsEval);
    return false

    };


/**
 * Development console
 *
 */
o2jse.lab = {

        contWin    : null,
        headBar    : null,
        sizeImg    : null,
        openImg    : null,
        logWin     : null,
        cmdBar     : null,
        cmdLine    : null,
        logBtn     : null,
        logStatus  : false,
        sqlBtn     : null,
        sqlStatus  : false,
        muteBtn    : null,
        muteStatus : false,
        searchTxt  : null,
        dPos       : {},   /* Delta positions while moving / sizing                     */
        winX       : 0,    /* Mouse pointer X while moving/sizing                       */
        winY       : 0     /* Mouse pointer Y while moving/sizing                       */

        };


/**
 * Create development console
 *
 */
o2jse.lab.init = function() {

    // ______________________________________________________________ Recreate console ___
    if (o2jse.lab.contWin) {
        o2jse.removeEl(o2jse.lab.contWin);
        o2jse.lab.contWin = null;
        }
    // __________________________________________________________________ External box ___
    o2jse.lab.contWin = o2jse.createEl(document.body, "div", "jxdbgwin");
    // ____________________________________________________________________ Header bar ___
    var hBar          = o2jse.createEl(o2jse.lab.contWin, "table");
    hBar.style.width  = "100%";
    hBar.cellSpacing  = "0";
    hBar.cellPadding  = "0";
    var hRow          = hBar.insertRow(-1);
    var sCell         = hRow.insertCell(-1);
    var hCell         = hRow.insertCell(-1);
    var oCell         = hRow.insertCell(-1);
    hCell.style.width = "100%";
    // _________________________________________________________________________ Sizer ___
    o2jse.lab.sizeImg                   = o2jse.createEl(sCell, "img");
    o2jse.lab.sizeImg.src               = o2jse.rntAlias + "img/dev/size.png";
    o2jse.lab.sizeImg.title             = "Drag to size";
    o2jse.lab.sizeImg.style.width       = "7px";
    o2jse.lab.sizeImg.style.height      = "7px";
    o2jse.lab.sizeImg.style.marginRight = "5px";
    o2jse.lab.sizeImg.style.cursor      = "pointer";
    // ________________________________________________________________________ Header ___
    o2jse.lab.headBar = o2jse.createEl(hCell, "div", "jxdbghead");
    // _____________________________________________________________ Open/close button ___
    o2jse.lab.openImg                  = o2jse.createEl(oCell, "img");
    o2jse.lab.openImg.src              = o2jse.rntAlias + "img/dev/close.png";
    o2jse.lab.openImg.style.width      = "7px";
    o2jse.lab.openImg.style.height     = "7px";
    o2jse.lab.openImg.style.marginLeft = "5px";
    o2jse.lab.openImg.style.cursor     = "pointer";
    // ____________________________________________________________________ Log screen ___
    o2jse.lab.logWin = o2jse.createEl(o2jse.lab.contWin, "div", "jxlogwin");
    o2jse.lab.logWin.innerHTML = "Hi " + o2jse.devName.substr(0,1).toUpperCase() +
                                 o2jse.devName.substr(1) + "!\n";
    // ______________________________________________________________________ Tool bar ___
    o2jse.lab.cmdBar = o2jse.createEl(o2jse.lab.contWin, "div", "jxcmdbar");
    // ___________________________________________________________________ Input field ___
    o2jse.lab.cmdLine = o2jse.createEl(o2jse.lab.contWin, "textarea", "jxcmdline");
    o2jse.lab.cmdLine.onkeydown = o2jse.lab.onKey;
    // _______________________________________________________________ Create tool bar ___
    var defBtn = function(srcUrl, toolTip, onClick) {

                    var btnObj;
                    // _________________________________________________ Create spacer ___
                    if (!toolTip) {
                        btnObj = o2jse.createEl(o2jse.lab.cmdBar,
                                                "div",
                                                "jxdbgsep",
                                                "&nbsp;");
                        }
                    // __________________________________________________ Create field ___
                    else if (!srcUrl) {
                        btnObj       = o2jse.createInput(o2jse.lab.cmdBar,
                                                         "text",
                                                         "jxdbgfield",
                                                         "",
                                                         "jxdbgfilter");
                        btnObj.title = toolTip;
                        }
                    // ___________________________________________ Create image button ___
                    else {
                        btnObj             = o2jse.createEl(o2jse.lab.cmdBar, "img");
                        btnObj.src         = o2jse.rntAlias + "img/dev/" + srcUrl;
                        btnObj.className   = "jxdbgbtn";
                        btnObj.jxClass     = "jxdbgbtn";
                        btnObj.title       = toolTip;
                        btnObj.onmouseover = function() { this.className =
                                                          this.jxClass+" jxdbgbto";};
                        btnObj.onmouseout  = function() { this.className = this.jxClass;};
                        btnObj.onclick     = onClick;
                        }
                    return btnObj;

                    };
    var btnClearC = function() { o2jse.lab.cmdLine.value = "";
                                 o2jse.lab.cmdLine.focus(); };
    var btnClearR = function() { o2jse.lab.logWin.innerHTML = ""; };
    // _______________________________________________________________________ Execute ___
    defBtn("exe.png", "Execute code", o2jse.lab.exe);
    // ________________________________________________________________________ Spacer ___
    defBtn();
    // _________________________________________________________________ Clear console ___
    defBtn("clear_down.png", "Clear console", btnClearC);
    // __________________________________________________________________ Clear result ___
    defBtn("clear_up.png", "Clear result screen", btnClearR);
    // ________________________________________________________________________ Spacer ___
    defBtn();
    // _________________________________________________________________ Elements list ___
    defBtn("select.png", "Variables list", o2jse.lab.varList);
    // __________________________________________________________________ Filter field ___
    o2jse.lab.searchTxt = defBtn(null, "Enter search keyword");
    // ________________________________________________________________________ Spacer ___
    defBtn();
    // ___________________________________________________________________________ Sql ___
    o2jse.lab.sqlBtn = defBtn("sql.png", "Start SQL log", o2jse.lab.sqlLog);
    // ___________________________________________________________________________ Log ___
    o2jse.lab.logBtn = defBtn("log.png", "Start log to file", o2jse.lab.fileLog);
    // _______________________________________________ Mute console (log only to file) ___
    o2jse.lab.muteBtn = defBtn("mute.png", "Mute (log only to file)", o2jse.lab.muteLog);
    // _________________________________________________________ Close first time only ___
    o2jse.lab.close();

    };


/**
 * Show development console
 *
 */
o2jse.lab.show = function() {

    if (!o2jse.lab.contWin) {
        o2jse.lab.init();
        }
    // ______________________________________________________________________ Show all ___
    o2jse.lab.logWin.style.display  = "block";
    o2jse.lab.cmdBar.style.display  = "block";
    o2jse.lab.cmdLine.style.display = "block";
    // ___________________________________________________________ Set console movable ___
    o2jse.lab.headBar.onmousedown   = function(e) { o2jse.lab.startMoving(this, e); };
    o2jse.lab.headBar.title         = "Drag to move";
    // ___________________________________________________________ Set console sizable ___
    o2jse.lab.sizeImg.style.display = "block";
    o2jse.lab.sizeImg.onmousedown   = function(e) { o2jse.lab.startSizing(this, e); };
    // __________________________________________________________ Set console closable ___
    o2jse.lab.openImg.style.display = "block";
    o2jse.lab.openImg.title         = "Click to close";
    o2jse.lab.openImg.onclick       = o2jse.lab.close;
    // ______________________________________________________ Set status-buttons value ___
    o2jse.lab.status(o2jse.lab.logStatus, o2jse.lab.sqlStatus, o2jse.lab.muteStatus);

    };


/**
 * Close down development console
 *
 */
o2jse.lab.close = function() {

    // __________________________________________________________________ Hide content ___
    o2jse.lab.headBar.onmousedown   = null;
    o2jse.lab.sizeImg.onmousedown   = null;
    o2jse.lab.contWin.style.height  = null;
    o2jse.lab.contWin.style.left    = null;
    o2jse.lab.contWin.style.top     = null;
    o2jse.lab.openImg.style.display = "none";
    o2jse.lab.sizeImg.style.display = "none";
    o2jse.lab.logWin.style.display  = "none";
    o2jse.lab.cmdBar.style.display  = "none";
    o2jse.lab.cmdLine.style.display = "none";
    // __________________________________________________________ Set console openable ___
    o2jse.lab.headBar.title         = "Development Console - Click to open";
    o2jse.lab.headBar.onclick       = o2jse.lab.show;

    };


/**
 * Execute code in cmdLine and show result text in logWin
 *
 * @param {Object} eventObj   Event object
 */
o2jse.lab.onKey = function(eventObj) {

    var stdEvent = o2jse.event.std(eventObj);
    // ___________________________________________________ If user send a "pure" ENTER ___
    if (stdEvent.keyCode == 13 &&
        !(stdEvent.shiftKey || stdEvent.ctrlKey || stdEvent.altKey)) {
        stdEvent.stop();
        o2jse.lab.exe();
        }

    };


/**
 * Execute code in cmdLine and show result text in logWin
 *
 */
o2jse.lab.exe = function() {

    o2jse.requester.exe("jxdev",
                        "jxcmdline=" + encodeURIComponent(o2jse.lab.cmdLine.value),
                        o2jse.lab.logWin,
                        o2jse.lab.getResult);

    };


/**
 * Request for variables list
 *
 */
o2jse.lab.varList = function() {

    o2jse.requester.exe("jxdev",
                        "jxvarlist=" + (o2jse.lab.searchTxt.value ?
                                        o2jse.lab.searchTxt.value : "1"),
                        o2jse.lab.logWin,
                        o2jse.lab.getResult);

    };


/**
 * Start/stop log to file
 *
 */
o2jse.lab.fileLog = function() {

    o2jse.requester.exe("jxdev",
                        "jxfilelog=" + (o2jse.lab.logStatus ? "off" : "on"),
                        o2jse.lab.logWin,
                        jxjs.jsEval);

    };


/**
 * Start/stop SQL log
 *
 */
o2jse.lab.sqlLog = function() {

    o2jse.requester.exe("jxdev",
                        "jxsqllog=" + (o2jse.lab.sqlStatus ? "off" : "on"),
                        o2jse.lab.logWin,
                        jxjs.jsEval);

    };


/**
 * Start/stop mute console log  - Log only to file
 *
 */
o2jse.lab.muteLog = function() {

    o2jse.requester.exe("jxdev",
                        "jxmutelog=" + (o2jse.lab.muteStatus ? "off" : "on"),
                        o2jse.lab.logWin,
                        jxjs.jsEval);

    };


/**
 * Set console status-buttons value
 *
 */
o2jse.lab.status = function(fileLog, sqlLog, muteLog) {

    if (!o2jse.lab.contWin) {
        o2jse.lab.init();
        }
    // ____________________________________________________________ Log to file status ___
    if (fileLog) {
        o2jse.lab.logBtn.className = "jxdbgbta";
        o2jse.lab.logBtn.jxClass   = "jxdbgbta";
        o2jse.lab.logBtn.title     = "Stop log to file";
        o2jse.lab.logStatus        = true;
        }
    else {
        o2jse.lab.logBtn.className = "jxdbgbtn";
        o2jse.lab.logBtn.jxClass   = "jxdbgbtn";
        o2jse.lab.logBtn.title     = "Start log to file";
        o2jse.lab.logStatus        = false;
        }
    // ____________________________________________________________ SQL tracing status ___
    if (sqlLog) {
        o2jse.lab.sqlBtn.className = "jxdbgbta";
        o2jse.lab.sqlBtn.jxClass   = "jxdbgbta";
        o2jse.lab.sqlBtn.title     = "Stop SQL log";
        o2jse.lab.sqlStatus        = true;
        }
    else {
        o2jse.lab.sqlBtn.className = "jxdbgbtn";
        o2jse.lab.sqlBtn.jxClass   = "jxdbgbtn";
        o2jse.lab.sqlBtn.title     = "Start SQL log";
        o2jse.lab.sqlStatus        = false;
        }
    // ___________________________________________________________ Mute console status ___
    if (muteLog) {
        o2jse.lab.muteBtn.className = "jxdbgbta";
        o2jse.lab.muteBtn.jxClass   = "jxdbgbta";
        o2jse.lab.muteBtn.title     = "Stop muting";
        o2jse.lab.muteStatus        = true;
        o2jse.lab.logWin.innerHTML += (o2jse.lab.logWin.innerHTML ? "<br>" : "") +
                                      "Logging on file";
        o2jse.lab.logWin.scrollTop  = o2jse.lab.logWin.scrollHeight;
        }
    else {
        o2jse.lab.muteBtn.className = "jxdbgbtn";
        o2jse.lab.muteBtn.jxClass   = "jxdbgbtn";
        o2jse.lab.muteBtn.title     = "Mute (log only to file)";
        o2jse.lab.muteStatus        = false;
        }

    };


/**
 * Add result text to logWin content
 *
 * @param {Object} logWin       Log window in the development console
 * @param {String} resultText   Text to add to log-win
 */
o2jse.lab.getResult = function(logWin, resultText) {

    if (!logWin) {
        if (!o2jse.lab.contWin) {
            o2jse.lab.init();
            }
        logWin = o2jse.lab.logWin;
        }
    o2jse.lab.show();
    logWin.innerHTML+= (logWin.innerHTML ? "<br>" : "") + resultText.decode();
    logWin.scrollTop = logWin.scrollHeight;

    };


/**
 * Open/close details for single log step
 *
 * @param {Object} dep    Detail element pointer object ([+])
 * @param {String} deid   Detail element ID - Unique ID for detail DIV
 */
o2jse.lab.ocd = function(dep, deid) {

    var detDiv = document.getElementById(deid);
    if (detDiv.style.display == "none") {
        dep.innerHTML = "[-]";
        detDiv.style.display = "block";
        }
    else {
        dep.innerHTML = "[+]";
        detDiv.style.display = "none";
        }

    };

/**
 * Open close details for single log step
 *
 * @param {String} elCode   Element code to add to cmd-line field
 */
o2jse.lab.addEl = function(elCode) {

    o2jse.lab.cmdLine.value+= elCode;

    };


/**
 * Starts action of console moving
 *
 * @param object targetObj   Object on which event is fired
 * @param object eventObj    On click event object
 */
o2jse.lab.startMoving = function(targetObj, eventObj) {

    o2jse.lab.contWin.style.position = "absolute";
    o2jse.lab.contWin.style.height = o2jse.lab.contWin.offsetHeight + "px";
    var stdEvent = o2jse.event.std(eventObj);
    if (stdEvent.button != 2) {
        stdEvent.stop();
        o2jse.win.dPos = {x:(o2jse.lab.contWin.offsetLeft - stdEvent.x),
                          y:(o2jse.lab.contWin.offsetTop - stdEvent.y)};
        o2jse.cmd.opacity(o2jse.lab.contWin, 80);
        o2jse.cmd.startGlass(o2jse.lab.moveTo, o2jse.lab.stopMoving, "move");
        }

    };


/**
 * Executes action of window moving
 *
 * @param object eventObj   Mouse move event object
 */
o2jse.lab.moveTo = function(eventObj) {

    var menuBarH = 0;
    var stdEvent = o2jse.event.std(eventObj);
    stdEvent.stop();
    if (o2jse.menu.appMainMenu && o2jse.menuStyle == 'T') {
        menuBarH = document.getElementById("jxMenuBar").offsetHeight;
        }
    var lX = (stdEvent.x + o2jse.win.dPos.x);
    var lY = Math.max(stdEvent.y + o2jse.win.dPos.y, menuBarH);
    if (o2jse.lab.contWin != null) {
        o2jse.lab.contWin.style.left = lX + 'px';
        o2jse.lab.contWin.style.top  = lY + 'px';
        }

    };


/**
 * Stops action of window moving
 *
 * @param object eventObj   Mouse up event object
 */
o2jse.lab.stopMoving = function(eventObj) {

    var stdEvent = o2jse.event.std(eventObj);
    stdEvent.stop();
    o2jse.cmd.stopGlass();
    o2jse.cmd.opacity(o2jse.lab.contWin, 100);
    o2jse.lab.dPos = null;

    };


/**
 * Starts action of window sizing
 *
 * @param object targetObj   Object on which event is fired
 * @param object eventObj    On click event object
 */
o2jse.lab.startSizing = function(targetObj, eventObj) {

    o2jse.ctrl.init(targetObj);
    var stdEvent = o2jse.event.std(eventObj);
    if (stdEvent.button != 2) {
        stdEvent.stop();
        o2jse.lab.dPos = {w: o2jse.lab.contWin.offsetWidth,
                          h: o2jse.lab.contWin.offsetHeight,
                          x:stdEvent.x,
                          y:stdEvent.y};
        o2jse.cmd.opacity(o2jse.lab.contWin, 80);
        o2jse.cmd.startGlass(o2jse.lab.sizeTo, o2jse.lab.stopSizing, "se-resize");
        }

    };


/**
 * Executes action of window sizing
 *
 * @param object eventObj   Mouse move event object
 */
o2jse.lab.sizeTo = function(eventObj) {

    var stdEvent = o2jse.event.std(eventObj);
    stdEvent.stop();
    o2jse.lab.contWin.style.left   = stdEvent.x + 'px';
    o2jse.lab.contWin.style.top    = stdEvent.y + 'px';
    o2jse.lab.contWin.style.width  = (o2jse.lab.dPos.w - stdEvent.x + o2jse.lab.dPos.x) +
                                     'px';
    o2jse.lab.contWin.style.height = (o2jse.lab.dPos.h - stdEvent.y + o2jse.lab.dPos.y) +
                                     'px';

    };


/**
 * Stops action of window sizing
 *
 * @param object eventObj   Mouse up event object
 */
o2jse.lab.stopSizing = function(eventObj) {

    var stdEvent = o2jse.event.std(eventObj);
    stdEvent.stop();
    o2jse.cmd.stopGlass();
    o2jse.cmd.opacity(o2jse.lab.contWin, 100);
    o2jse.lab.dPos = {};
    // ____________________________________________________ Set internal elements size ___
    o2jse.lab.logWin.style.height = (o2jse.lab.contWin.offsetHeight -
                                     o2jse.lab.logWin.offsetTop -
                                     o2jse.lab.cmdLine.offsetHeight - 30) + "px";

    };
