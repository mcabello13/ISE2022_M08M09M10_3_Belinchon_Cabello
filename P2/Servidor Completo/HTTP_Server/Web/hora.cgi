t <html><head><title>Hora Fecha</title>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
t  adh = document.getElementById("ad_hora").value;
t  document.getElementById("ad_hora").value = (adh);
t </script></head>
i pg_header.inc
t <h2 align=center><br>Control de Hora SNTP</h2>
t <p><font size="2">En esta pagina se muestra la <b>hora y la fecha proporcionadas por SNTP</b> 
t   <td align="center">
t <input type="text" readonly style="background-color: transparent; border: 0px"
c m  size="10" id="ad_hora" value="%.2d:%.2d:%.2d"></td>
. End of script must be closed with period.


