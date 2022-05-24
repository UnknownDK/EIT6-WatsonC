clear
close all


Fs = 21;
t = linspace(0, 1, Fs+1);
f = 4;
phase_shift = pi;

sig1 = zeros(size(t));
sig2 = zeros(size(t));

sig1(t > 0.5) = 1.0 * sin(2 * pi * f * t(t > 0.5)); %+ 0.1 * sin(2 * pi * (3*f * t + rand()));
sig2(t > 0.5) = 0.8 * sin(2 * pi * f * t(t > 0.5) + phase_shift);% + 0.1 * sin(2 * pi * (3*f * t + rand())); 


x = xcorr( sig1, sig2, 'coeff'); 
[foo, ndx] = max( x);
tx = linspace( -t(end), t(end), 2*length(t) -1);
time_lag = tx(ndx)


t_i = linspace(0, 1, 1001);
sig1_i = interp1(t, sig1, t_i);
sig2_i = interp1(t, sig2, t_i);
x_i = xcorr( sig1_i, sig2_i, 'coeff'); 
[foo_i, ndx_i] = max(x_i);
tx_i = linspace( -t_i(end), t_i(end), 2*length(t_i) -1);
time_lag_i = tx_i(ndx_i)

%
figure;

subplot(3,1,1);
plot(t, sig1, 'o', t, sig2, 'o', t_i, sig1_i, t_i, sig2_i);
title('Sampled Signals');
xlabel('Time [s]');

subplot(3,1,2); 
plot(tx, x);
xlabel('Time [s]');

subplot(3,1,3); 
plot(t, sig1, t+tx(ndx), sig2, t_i+tx_i(ndx_i), sig2_i)
title('Phase shifted w. cross correlation');
xlim([0 1]);
xlabel('Time [s]');
