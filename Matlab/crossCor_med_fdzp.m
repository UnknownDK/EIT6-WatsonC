clear 
close all

snr_n = 120;
upscaleFactor = 4;
L = 128;             % Antal samples
Fs = 96000;            % Sampling frequency      
f = 40;             % Signal freq
T = 1/Fs;             % Sampling period for 1 sample       
t = (0:L-1)*T;        % Time vector

fakeSignal = [zeros(1,30) 0 0.0119324 -0.0206909 0.0238953 -0.0206909 0.0119324 0 -0.0119324 0.0206604 -0.0238953 0.0206909 -0.0119324 0 0.0119324 -0.0206909 0.0238953 -0.0206909 0.0119324 0 -0.0119324 0.0206909 -0.0238953 zeros(1,128-52)];
%fakeSignal2 = [zeros(1,31) 0 0.0119324 -0.0206909 0.0238953 -0.0206909 0.0119324 0 -0.0119324 0.0206604 -0.0238953 0.0206909 -0.0119324 0 0.0119324 -0.0206909 0.0238953 -0.0206909 0.0119324 0 -0.0119324 0.0206909 0 zeros(1,29)];

% defineret fra tid
% tid = 0.0016;
% phase_shift = tid*(f*pi*2);

% defineret fra faseskift
phase_shift = 0.62*pi;
tid = phase_shift/(f*pi*2);



sig1PN = zeros(size(t));
sig2PN = zeros(size(t));


%OldStoej

% stoejElements = 500;
% amp = 0.05;
% stoejFreqMax = 30e6;
% a_stoej = -amp + (amp+amp) * rand(1, stoejElements);
% f_stoej = 1 + (stoejFreqMax-1) * rand(1, stoejElements);
% p_stoej = 0 + (pi-0) * rand(1, stoejElements);
% a_stoej2 = -amp + (amp+amp) * rand(1, stoejElements);
% f_stoej2 = 1 + (stoejFreqMax-1) * rand(1, stoejElements);
% p_stoej2 = 0 + (pi-0) * rand(1, stoejElements);
% 
% stoej = zeros(stoejElements,L);
% stoej2 = zeros(stoejElements,L);
% for i = 1:stoejElements
%     stoej(i,:) =   a_stoej(i) * sin((2 * pi * t * f_stoej(i))+ p_stoej(i));
% end
% for i = 1:stoejElements
%     stoej2(i,:) =   a_stoej2(i) * sin((2 * pi * t * f_stoej2(i))+ p_stoej2(i));
% end
% stoejSum = sum(stoej);
% stoejSum2 = sum(stoej2);


%sig1 = 1.0 * sin(2 * pi * f * t); 
%sig2 = 0.8 * sin(2 * pi * f * t + phase_shift);

% sig1PN(t > (max(t)/4)) = 1.0 * sin(2 * pi * f * t(t > (max(t)/4))); %+ 0.1 * sin(2 * pi * (3*f * t + rand()));
% sig2PN(t > (max(t)/4)) = 0.8 * sin((2 * pi * f * t(t > (max(t)/4))) + phase_shift);% + 0.1 * sin(2 * pi * (3*f * t + rand())); 
for i = 2*(L/4):1:(3*L/4)
    sig1PN(i) = 1.0 * sin(2 * pi * f * t(i));
    sig2PN(i) = 1.0 * sin(2 * pi * f * t(i)+phase_shift);
end


% tilfoej stoej
% sig1 = sig1+stoejSum; %old
% sig2 = sig2+stoejSum;
sig1 = awgn(sig1PN,snr_n,'measured');
sig2 = awgn(sig2PN,snr_n,'measured');

sig2=fakeSignal;


%correlation
x = xcorr( sig1, sig2, 'coeff'); 
[foo, ndx] = max(x);
tx = linspace( -t(end), t(end), 2*length(t) -1);
time_lag = tx(ndx);

% %extra correlation
% L_i = L*500;
% T_i = 1/(Fs*500);
% t_i  = (0:L_i-1)*T_i;
% 
% sig1_i = interp1(t, sig1, t_i);
% sig2_i = interp1(t, sig2, t_i);
% x_i = xcorr( sig1_i, sig2_i, 'coeff'); 
% [foo_i, ndx_i] = max(x_i);
% tx_i = (0:L_i-1)*T_i;
% time_lag_i = tx_i(ndx_i)

%Plot af foerste correlation
ogF = figure;
ogF.Name = 'Oprindelig signaler';
ogF.Position(1:2) = [80, 100];

subplot(3,1,1);
plot(t, sig1, t, sig2)%, 'o', t_i, sig1_i, t_i, sig2_i);
title('Sampled Signals');
xlabel('Time [s]');

subplot(3,1,2); 
plot(tx, x);
xlabel('Time [s]');

subplot(3,1,3); 
plot(t, sig1, t+tx(ndx), sig2)%, t_i+tx_i(ndx_i), sig2_i)
title('Phase shifted w. cross correlation');
%xlim([0 1]);
xlabel('Time [s]');



%zeroPadding
%Timesteps for upscaling
L2 = L*upscaleFactor;
T2 = 1/(Fs*upscaleFactor);
t2 = (0:L2-1)*T2;

n = 2^nextpow2(L);
freq_sig1 = fft(sig1,n);
freq_sig2 = fft(sig2,n);

%plot FFT
fftF = figure;
fftF.Name = 'DFT af signaler';
fftF.Position(1:2) = [800, 100];

freq = Fs*(0:n-1)/n;
P1 = abs(freq_sig1);
P2 = abs(freq_sig2);
plot(freq,P1,'-*',freq,P2,'-o')
grid


%FDZP
freq_sig1_pad = [freq_sig1(1:L/2) zeros(1,L*(upscaleFactor-1)) freq_sig1((L/2)+1:end)];
sig1_Pad = real(ifft(freq_sig1_pad));
freq_sig2_pad = [freq_sig2(1:L/2) zeros(1,L*(upscaleFactor-1)) freq_sig2((L/2)+1:end)];
sig2_Pad = real(ifft(freq_sig2_pad));

%correlation af fdzp signal
x_pad = xcorr( sig1_Pad, sig2_Pad, 'coeff'); 
[foo, ndx_pad] = max(x_pad);
tx_2 = linspace( -t2(end), t2(end), 2*length(t2) -1);
time_lag_pad = tx_2(ndx_pad);

%Plot af fdzp
newF = figure();
newF.Name = 'UpscaledFigur';
newF.Position(1:2) = [600, 300];

subplot(3,1,1);
plot(t2, sig1_Pad, t2, sig2_Pad)%, 'o', t_i, sig1_i, t_i, sig2_i);
title('Sampled Signals');
xlabel('Time [s]');

subplot(3,1,2); 
plot(tx_2, x_pad);
xlabel('Time [s]');

subplot(3,1,3); 
plot(t2, sig1_Pad, t2+tx_2(ndx_pad), sig2_Pad)%, t_i+tx_i(ndx_i), sig2_i)
title('Phase shifted w. cross correlation');
%xlim([0 1]);
xlabel('Time [s]');

dif = abs(tid-time_lag)
dif_pad = abs(tid-time_lag_pad)




%plot FFT2
n2 = 2^nextpow2(L*upscaleFactor);
Tfreq_sig1 = fft(sig1_Pad,n2);
Tfreq_sig2 = fft(sig2_Pad,n2);

fftF2 = figure;
fftF2.Name = 'DFT af fdzp signaler';
fftF2.Position(1:2) = [800, 200];

tfreq = Fs*upscaleFactor*(0:n2-1)/n2;
tP1 = abs(Tfreq_sig1);
tP2 = abs(Tfreq_sig2);
plot(tfreq,tP2,'-o')
grid