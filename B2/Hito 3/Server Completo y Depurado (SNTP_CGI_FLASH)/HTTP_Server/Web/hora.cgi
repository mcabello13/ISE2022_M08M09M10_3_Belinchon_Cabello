t <html><head><title>Button inputs</title>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
# Define URL and refresh timeout
t var formUpdate = new periodicObj("hora.cgx", 500);
t function plotRTCGraph() {
t  rtcVal = document.getElementById("hora").value;
t  rtcFech = document.getElementById("fecha").value;
t }
t function periodicUpdateRTC() {
t  if(document.getElementById("refreshChkBox").checked == true) {
t   updateMultiple(formUpdate,plotRTCGraph);
t   periodicFormTime = setTimeout("periodicUpdateRTC", formUpdate.period); 
t  }
t  else
t   clearTimeout(periodicFormTime);
t }
t </script></head>
i pg_header.inc
t <h3 align="center"><br>DATE & HOUR on the board</h3>
t  Se muestran la hora y fecha actuales.</font></p>
t <form action=hora.cgi method=post name=cgi>
t <input type=hidden value="hora" name=pg>
t <table border="0" width=99%><font size="3">
t <tr bgcolor=#aaccff>
t  <th width=40%>Item</th>
t  <th width=60%>Fecha y Hora</th></tr>
# Here begin data setting which is formatted in HTTP_CGI.C module
t <tr><td><img src="pabb.gif">Hora:</td>
c h 1 <td><input type=text readonly id="hora" value="%s" size=20 maxlength=20 ></td></tr>
t <tr><td><img src="pabb.gif">Fecha:</td>
c h 2 <td><input type=text readonly id="fecha" value="%s" size=20 maxlength=20 ></td></tr>
t </font></table>
# Here begin button definitions
t <p align="center">
t <input type=submit name=refresh value="Refresh" id="ise">
t </p></form>
i pg_footer.inc
. End of script must be closed with period.
