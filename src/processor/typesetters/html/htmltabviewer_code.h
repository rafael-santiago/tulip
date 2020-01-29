/*
 *                           Copyright (C) 2005-2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef TULIP_PROCESSOR_TYPESETTERS_HTML_HTMLTABVIEWER_CODE_H
#define TULIP_PROCESSOR_TYPESETTERS_HTML_HTMLTABVIEWER_CODE_H 1

#define HTMLTYPESETTER_TAB_VIEWER_CODE_BEGIN "<HTML>\n"\
                                             "<HEAD>\n"\
                                             "<META name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"\
                                             "<STYLE>\n"\
                                             "* {box-sizing: border-box}\n"\
                                             "body {font-family: Courier, sans-serif; margin:0}\n"\
                                             ".tabReader {display: none}\n"\
                                             "img {vertical-align: middle;}\n"\
                                             ".tablature-container {\n"\
                                             "  max-width: 1000px;\n"\
                                             "  position: relative;\n"\
                                             "  margin: auto;\n"\
                                             "}\n"\
                                             ".prev, .next {\n"\
                                             "  cursor: pointer;\n"\
                                             "  position: absolute;\n"\
                                             "  top: 50%;\n"\
                                             "  width: auto;\n"\
                                             "  padding: 16px;\n"\
                                             "  margin-top: -22px;\n"\
                                             "  color: black;\n"\
                                             "  font-weight: bold;\n"\
                                             "  font-size: 18px;\n"\
                                             "  transition: 0.6s ease;\n"\
                                             "  border-radius: 0 3px 3px 0;\n"\
                                             "  user-select: none;\n"\
                                             "}\n"\
                                             ".next {\n"\
                                             "  right: 0;\n"\
                                             "  border-radius: 3px 0 0 3px;\n"\
                                             "}\n"\
                                             ".prev:hover, .next:hover {\n"\
                                             "  color: white;\n"\
                                             "  background-color: rgba(0,0,0,0.8);\n"\
                                             "}\n"\
                                             ".page-number {\n"\
                                             "  color: #000000;\n"\
                                             "  font-size: 12px;\n"\
                                             "  width: 100%;\n"\
                                             "  padding: 8px 12px;\n"\
                                             "  position: absolute;\n"\
                                             "  top: 8px;\n"\
                                             "  text-align: right;\n"\
                                             "}\n"\
                                             ".dot {\n"\
                                             "  cursor: pointer;\n"\
                                             "  height: 15px;\n"\
                                             "  width: 15px;\n"\
                                             "  margin: 0 2px;\n"\
                                             "  background-color: #bbb;\n"\
                                             "  border-radius: 50%;\n"\
                                             "  display: inline-block;\n"\
                                             "  transition: background-color 0.6s ease;\n"\
                                             "}\n"\
                                             ".active, .dot:hover {\n"\
                                             "  background-color: #717171;\n"\
                                             "}\n"\
                                             ".fade {\n"\
                                             "  -webkit-animation-name: fade;\n"\
                                             "  -webkit-animation-duration: 1.5s;\n"\
                                             "  animation-name: fade;\n"\
                                             "  animation-duration: 1.5s;\n"\
                                             "}\n"\
                                             "@-webkit-keyframes fade {\n"\
                                             "  from {opacity: .4}\n"\
                                             "  to {opacity: 1}\n"\
                                             "}\n"\
                                             "@keyframes fade {\n"\
                                             "  from {opacity: .4}\n"\
                                             "  to {opacity: 1}\n"\
                                             "}\n"\
                                             "@media only screen and (max-width: 300px) {\n"\
                                             "  .prev, .next, .text {font-size: 11px}\n"\
                                             "}\n"\
                                             "</STYLE>\n"\
                                             "</HEAD>\n"\
                                             "<BODY>\n"\
                                             "<DIV class=\"tablature-container\">\n"

#define HTMLTYPESETTER_TAB_VIEWER_CODE_MIDDLE "</DIV>\n"\
                                              "<A CLASS=\"prev\" onclick=\"incPage(-1)\">&#10094;</A>\n"\
                                              "<A CLASS=\"next\" onclick=\"incPage(1)\">&#10095;</A>\n"\
                                              "</DIV>\n"\
                                              "<BR>\n"\
                                              "<DIV style=\"text-align:center\">\n"

#define HTMLTYPESETTER_TAB_VIEWER_CODE_END "</DIV>\n"\
                                           "<SCRIPT type=\"text/javascript\">\n"\
                                           "var pageIndex = 1;\n"\
                                           "showPage(pageIndex);\n"\
                                           "function incPage(n) {\n"\
                                           "  showPage(pageIndex += n);\n"\
                                           "}\n"\
                                           "function currentPage(n) {\n"\
                                           "  showPage(pageIndex = n);\n"\
                                           "}\n"\
                                           "function showPage(n) {\n"\
                                           "  var pages = document.getElementsByClassName(\"tabReader\");\n"\
                                           "  var dots = document.getElementsByClassName(\"dot\");\n"\
                                           "  if (n > pages.length) {\n"\
                                           "    pageIndex = 1;\n"\
                                           "  }\n"\
                                           "  if (n < 1) {\n"\
                                           "    pageIndex = pages.length;\n"\
                                           "  }\n"\
                                           "  for (p = 0; p < pages.length; p++) {\n"\
                                           "      pages[p].style.display = \"none\";\n"\
                                           "  }\n"\
                                           "  for (d = 0; d < dots.length; d++) {\n"\
                                           "      dots[d].className = dots[d].className.replace(\" active\", \"\");\n"\
                                           "  }\n"\
                                           "  pages[pageIndex-1].style.display = \"block\";\n"\
                                           "  dots[pageIndex-1].className += \" active\";\n"\
                                           "}\n"\
                                           "document.onkeydown = function(event) {\n"\
                                           "  event = event || window.event;\n"\
                                           "  if (event.keyCode == \"37\") {\n"\
                                           "    incPage(-1);\n"\
                                           "  } else if (event.keyCode == \"39\") {\n"\
                                           "    incPage(1);\n"\
                                           "  }\n"\
                                           "};"\
                                           "</SCRIPT>\n"\
                                           "</BODY>\n"\
                                           "</HTML>\n"
#endif
