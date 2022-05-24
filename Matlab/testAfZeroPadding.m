clear
close all

figure(1)

Fs = 96;            % Sampling frequency                    
T = 1/Fs;             % Sampling period       
L = 64;             % Length of signal
t = (0:L-1)*T;        % Time vector

upscaleFactor = 2;

%Timesteps for upscaling
L2 = L*upscaleFactor;
T2 = 1/(Fs*upscaleFactor);
t2 = (0:L2-1)*T2;

L3 = L*500;
T3 = 1/(Fs*500);
t3 = (0:L3-1)*T3;

Freq = 40;
Freq2 = 8.7;
OG = 0.8*sin(Freq*2*pi*t);%+cos(Freq2*2*pi*t); %signal
OG_highRes = cos(Freq*2*pi*t3)+cos(Freq2*2*pi*t3); %signal



hold on

new = upsample(OG,upscaleFactor);


% [b, a] = butter(12,0.5);
% filtered = filter(b, a, new);
% plot(t2,filtered)

hold off
figure(2)


n = 2^nextpow2(L);
freqOG = fft(OG,n);
%f = Fs*(0:n-1)/n;      %for hz
f = 2*pi*(0:n-1)/n;     %for pi
P = abs(freqOG);
plot(f,P,'--','LineWidth',2)
grid
hold on

%figure()
n2 = 2^nextpow2(L2);
freqNew = fft(new,n2);
%f2 = (Fs*upscaleFactor)*(0:n2-1)/n2; %for hz
f2 = 2*pi*upscaleFactor*(0:n2-1)/n2;  %for pi
P2 = abs(freqNew);
plot(f2,P2, '-') 

%xline(f((n/2)+1),'--r','pi');
xline(f2((n2/2)+1),'--r','upsampled \pi');
xlabel('Frequency [rad/s]')
xlim([0 4*pi])
set(gca,'XTick',0:pi/2:4*pi)
set(gca,'XTickLabel',{'0','\pi/2','\pi','3\pi/2','2\pi','5\pi/2','3\pi','7\pi/2','4\pi'})
hold off


% test = zeros(1,upscaleFactor*length(freqOG));
% test([1:upscaleFactor:end]) = freqOG;
% timePadded = ifft(test);

%freqNewPad = [zeros(1,(L*3)/2) freqOG zeros(1,(L*3)/2)];
%timePadded = ifft(freqNewPad);

freqNewPad = [freqOG(1:L/2) zeros(1,L*(upscaleFactor-1)) freqOG((L/2)+1:end)];
timePadded = ifft(freqNewPad);



figure(1)
hold on
plot(t,OG)
%plot(t2,new)
plot(t2,timePadded*upscaleFactor)

plot(t3,OG_highRes, '--')
grid
hold off

% filteredFFT = fft(filtered,n2);
% P3 = abs(filteredFFT);
% plot(f2,P3, '-') 





