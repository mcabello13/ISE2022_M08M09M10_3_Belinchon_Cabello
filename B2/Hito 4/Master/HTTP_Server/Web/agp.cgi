t <html><head><title>AGP Control</title>
t <script language=JavaScript>
t </script></head>
i pg_header.inc
t <h2 align=center><br>AGP on the board</h2>
t <p><font size="2">Se puede programar una <b>ganancia<b> para el <b>A.Instrumentacion, un 
t nivel de <b>Overload (introduzca valor y pulse enter) y activar la Int. para Overload.</font></p>
t <form action=agp.cgi method=post name=form2>
t <input type=hidden value="agp" name=pg2>
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#27e910  >
t  <th width=40%>Item</th>
t  <th width=60%>Setting</th></tr>
t <td><img src=pabb.gif>Ganancia(V/V):</td>
t <td><select name="ctrl" id="opcion" onchange="submit();">
c w c <option %s>Seleccionar</option><option %s>1</option>
c w c <option %s>5</option><option %s>10</option>
c w c <option %s>50</option><option %s>100</option></select></td></tr>
# Here begin data setting which is formatted in HTTP_CGI.C module
t <tr><td><img src=pabb.gif>Overload [0..10]</td></tr>
c w d <td><input type=text name=over1 size=20 maxlength=20 value="%s"></td></tr>
t </font></table>
t <td><table><tr valign="middle">
t <td><img src=pabb.gif>Int. Overload(S/N):</td>
t <td><select name="ctr" id="opcion" onchange="submit();">
c w e <option %s>Seleccionar</option><option %s>Si</option>
c w e <option %s>No</option></select></td></tr>
i pg_footer.inc
. End of script must be closed with period.