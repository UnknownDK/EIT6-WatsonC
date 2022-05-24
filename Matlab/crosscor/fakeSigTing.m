clear 
close all


fakeSignal2 = [zeros(1,30) 0 0.0119324 -0.0206909 0.0238953 -0.0206909 0.0119324 0 -0.0119324 0.0206604 -0.0238953 0.0206909 -0.0119324 0 0.0119324 -0.0206909 0.0238953 -0.0206909 0.0119324 0 -0.0119324 0.0206909 zeros(1,128-51)];


upscaleFactor = 8;
L = 128;             % Antal samples
Fs = 96000;            % Sampling frequency      
f = 40000;             % Signal freq
T = 1/Fs;             % Sampling period for 1 sample   
t = (0:L-1)*T;        % Time vector

FS2 = Fs*upscaleFactor;
T2 = 1/FS2;
t2 = (0:L*upscaleFactor-1)*T2
A = 21;

sig1 = zeros(size(A*upscaleFactor));


for i = 1:1:(A*upscaleFactor)
    sig1(i) =  0.0246484375* sin(2 * pi * f * t2(i));
end


% %correlation
% x = xcorr( sig1, sig2, 'coeff')
% [foo, ndx] = max(x)
% 
% tx = linspace( -t(end), t(end), 2*length(t) -1);
% time_lag = tx(ndx);


n = 2^nextpow2(L);
freq_sig2 = fft(fakeSignal2,n);

% %plot FFT
% fftF = figure;
% fftF.Name = 'DFT af signaler';
% fftF.Position(1:2) = [800, 100];

% freq = Fs*(0:n-1)/n;
% P1 = abs(freq_sig1);
% P2 = abs(freq_sig2);
% plot(freq,P1,'-*',freq,P2,'-o')
% grid


%FDZP
freq_sig2_pad = [freq_sig2(1:L/2) zeros(1,L*(upscaleFactor-1)) freq_sig2((L/2)+1:end)];
sig2_Pad = upscaleFactor*real(ifft(freq_sig2_pad));


x = xcorr(sig2_Pad, sig1, 'none')
[foo, ndx] = max(x)



tx = linspace( -t2(end), t2(end), 2*length(t2) -1);
time_lag = tx(ndx)
%svarer til:
%  (index-((x_length + 1) /2)/fs
timeLag = (ndx-((length(x)+1)/2))/(Fs*upscaleFactor)

plot(t2, [sig1 zeros(1,(length(sig2_Pad)-length(sig1)))], t2-tx(ndx), sig2_Pad )%, t_i+tx_i(ndx_i), sig2_i)
title('Phase shifted w. cross correlation');
%xlim([0 1]);
xlabel('Time [s]');
