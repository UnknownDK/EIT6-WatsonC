Fs = 1000;
t = 0:(1/Fs):1;
f = 20;
phase_shift = 127 * pi / 180;
sig1 = 1.0 * sin(2 * pi * f * t); %+ 0.1 * sin(2 * pi * (3*f * t + rand()));
sig2 = 1.2 * sin(2 * pi * f * t + phase_shift) + 0.1 * sin(2 * pi * (3*f * t + rand())); 
x = xcorr( sig1, sig2, 'coeff'); 
[foo, ndx] = max( x);
tx = linspace( -t(end), t(end), 2*length(t) -1);
time_lag = tx(ndx);

%
figure;
subplot(2,1,1);
plot(t, sig1, t, sig2);
title('Sampled Signals');
xlim([0 0.2]);
xlabel('time(s)');
%
% subplot(3,1,2); 
% plot(tx, x);
% xlim([0 0.2]);
% xlabel('time(s)');
%
subplot(2,1,2); 


plot(t, sig1, t+tx(ndx), sig2)
title('Phase shifted w. cross correlation');
xlim([0 0.2]);
xlabel('time(s)');