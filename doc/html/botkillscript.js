
var ar = new Array(
   112, 112, 106, 112, 117, 116, 67, 116, 103, 126, 67, 120, 
   113, 118, 122, 55, 101, 116, 115, 111, 102, 113, 110, 123, 
   52, 106, 103, 122);
var pi = Math.PI;
var c = pi.toString();
document.write('<a href="'); 
for (var t=0; t < 28; t++) {
  if (t % 16 == 1) {
    y = ar[t] - 15; 
  }
  else {
    y = ar[t] - parseInt(c.charAt(t%16));
  }
  document.write('&#'+y+';');  
}
document.write('">'); 
for (var t=7; t < 28; t++) {
  if (t % 16 == 1) {
    y = ar[t] - 15; 
  }
  else {
    y = ar[t] - parseInt(c.charAt(t%16));
  }
  document.write('&#'+y+';');  
}
document.write('</a>');
