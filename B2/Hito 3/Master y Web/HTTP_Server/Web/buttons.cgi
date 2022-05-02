t <html><head><title>Fecha y Hora</title>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
# Define URL and refresh timeout
t var formUpdate = new periodicObj("buttons.cgx", 1000);
t function plotRTCGraph() {
t  rtcVal = document.getElementById("hora").value;
t  rtDate = document.getElementById("fecha").value;
t }
t function periodicUpdateRTC() {
t  if(document.getElementById("rtcChkBox").checked == true) {
t   updateMultiple(formUpdate,plotRTCGraph);
t   rtc_elTime = setTimeout(periodicUpdateRTC, formUpdate.period);
t  }
t  else
t   clearTimeout(rtc_elTime);
t }
t </script></head>
i pg_header.inc
t <h2 align=center><br>Fecha y Hora</h2>
t <p><font size="2">Se muestan la hora y la fecha con protocolo SNTP.
t <form action=buttons.cgi method=post name=cgi>
t <input type=hidden value="reloj" name=pg>
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#aaccff>
t  <th width=40%>Item</th> 
t  <th width=60%>Valor</th></tr>
# Here begin data setting which is formatted in HTTP_CGI.C module
t <tr><td><img src=pabb.gif>Hora</td>
c w 1 <td><input type=text readonly id="hora" size=20 maxlength=20 ></td></tr>
t <tr><td><img src=pabb.gif>Fecha</td>
c w 2 <td><input type=text readonly id="fecha" size=20 maxlength=20 ></td></tr>
t </font></table>
# Here begin button definitions 
t <p align=center>
t Activar:<input type="checkbox" id="rtcChkBox" onclick="periodicUpdateRTC()">
t </p></form>
i pg_footer.inc
. End of script must be closed with period. 
