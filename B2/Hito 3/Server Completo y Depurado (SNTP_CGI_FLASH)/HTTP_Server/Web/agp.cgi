t <html><head><title>AGP Control</title>
t <script language=JavaScript>
t function ganancias(st) {
t  for(i=0;i<document.form1.length;i++) {
t   if(document.form1.elements[i].type=="checkbox"){
t    document.form1.elements[i].checked=st;
t   }
t  }
t  document.form1.submit();
t }
t </script></head>
i pg_header.inc
t <h2 align="center"><br>AGP on the board</h2>
t  Se puede programar la <b>ganancia<b> para el amplificador de instrumentación<b>, 
t  a elegir entre <b>1, 5, 10, 50 y 100<b>, y el umbral para el <b>overload<b>.</font></p>
t <form action="agp.cgi" method="post" name="agp">
t <input type="hidden" value="agp" name="pg">
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#58d68d>
t  <th width=40%>Item</th>
t  <th width=60%>Options</th></tr>
t <td><img src=pabb.gif>Ganancia:</td>
t <td><select name="ganancias "onchange="">
c i 1 <option %s>5</option><option %s>10</option>
t <option %s>50</option><option %s>100</option></select></form></td></tr>
t <tr><td><img src=pabb.gif>Overload [-10..10]:</td>
c i 2 <td><input type=text name=overload size=2 maxlength=2 value="%s"></td></tr>
t <tr><td><img src=pabb.gif>Ganancia:</td>
t <td><table><tr valign="middle">
# Here begin the 'checkbox' definitions
t <td width="5%"></td>
c i 3 <td><input type=checkbox name=g3 OnClick="submit" %s>100</td>
c i 2 <td><input type=checkbox name=g2 OnClick="submit" %s>50</td>
c i 1 <td><input type=checkbox name=g1 OnClick="submit" %s>10</td>
c i 0 <td><input type=checkbox name=g0 OnClick="submit" %s>5</td>
t </font></table>
t </table></form>
# Here begin button definitions
t <p align=center>
t <input type=submit name=set value="Send" id="ov">
t </p></form>
i pg_footer.inc
. End of script must be closed with period.
