
a = 1.99914767;
b = 3.199734756*10^-10;
frequency = 40000;

d = linspace(1, 50);

wtr_prop_att =  (b*frequency^(a)/1000);

y = 20*log10(1./d) - d*wtr_prop_att;

plot(m, y);