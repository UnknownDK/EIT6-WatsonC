%% crosscorr
%fakeSignal = [0 0 0 0 0 0 0 0 0 0 0 0.1875 -0.320312 0.375 -0.320312 0.1875 0 -0.1875 0.320312 -0.375 0.320312 -0.179688 0 0.1875 -0.320312 0.375 -0.320312 0.179688 0 -0.1875 0.320312 -0.375 ];
%compareSignal = [ 0 0 0 0 0 0 0.1875 -0.320312 0.375 -0.320312 0.1875 0 -0.1875 0.320312 -0.375 0.320312 ];

clear

fakeSignal = [0 0.0119324 -0.0206909 0.0238953 -0.0206909 0.0119324 0 -0.0119324 0.0206604 -0.0238953 0.0206909 -0.0119324 0 0.0119324 -0.0206909 0.0238953 -0.0206909 0.0119324 0 -0.0119324 0.0206909 ];%-0.0238953
fakeSignal2 = [zeros(1,31) 0 0.0119324 -0.0206909 0.0238953 -0.0206909 0.0119324 0 -0.0119324 0.0206604 -0.0238953 0.0206909 -0.0119324 0 0.0119324 -0.0206909 0.0238953 -0.0206909 0.0119324 0 -0.0119324 0.0206909 0 zeros(1,29)];

snr_n = 70;
upscaleFactor = 16;
L = length(fakeSignal2);             % Antal samples
Fs = 96000;            % Sampling frequency      
f = 40000;             % Signal freq
T = 1/Fs;             % Sampling period for 1 sample       
t = (0:L-1)*T;        % Time vector
phase_shift = 0.62*pi;
tid = phase_shift/(f*pi*2);

%fakeSignal = [0 0 0 0 0 0 0 0 0 0 0 0.0119324 -0.0206909 0.0238953 -0.0206909 0.0119324 0 -0.0119324 0.0206604 -0.0238953 0.0206909 -0.0119324 0 0.0119324 -0.0206909 0.0238953 -0.0206909 0.0119324 0 -0.0119324 0.0206909 -0.0238953 ];


%compareSignal = [ 0 0 0 0 0 0 0.0119324 -0.0206909 0.0238953 -0.0206909 0.0119324 0 -0.0119324 0.0206604 -0.0238953 0.0206909 ];




x = xcorr(fakeSignal2, fakeSignal, 'none')
[foo, ndx] = max(x)



tx = linspace( -t(end), t(end), 2*length(t) -1);
time_lag = tx(ndx)
%svarer til:
%  (index-((x_length + 1) /2)/fs
timeLag = (ndx-((length(x)+1)/2))/Fs

plot(t, [fakeSignal zeros(1,(length(fakeSignal2)-length(fakeSignal)))], t-tx(ndx), fakeSignal2 )%, t_i+tx_i(ndx_i), sig2_i)
title('Phase shifted w. cross correlation');
%xlim([0 1]);
xlabel('Time [s]');



% indices = find(abs(x)<0.005);
% x(indices) = [];




%% FFT bitreverse ting
clear
%de2bi(31145984)

%a = [ 0.7 -0.3 0.2 -0.5 0.6 -0.2 0.3 -0.5 0.7 -0.3 0.2 -0.5 0.6 -0.2 0.3 -0.5 ];
a = [-0.008469008840620518, 0.021574726328253746,...
0.025534227956086397, 0.020622544456273317,...
0.012893986888229847, -0.014209831599146128,...
0.0036056428216397762, -0.03602507896721363,...
0.010494280606508255, 0.027948909904807806,...
-0.007938717026263475, 0.00610621552914381,...
0.013380493503063917, 0.0003175572492182255...
0.004195366520434618, -0.006343390792608261...
0.009765237104147673, -0.02398272743448615,...
-0.01636465545743704, 0.0034589548595249653...
0.023994849529117346, -0.021476229187101126...
0.00678790220990777, -0.013113836292177439,...
-0.02995524788275361, -0.003260040190070867...
0.007342805154621601, 0.011811140459030867,...
0.019911736249923706, 0.005870850756764412,...
0.009977225679904222, -0.021022077649831772 ];

n = 1;
for j= 1:2:31
    c(n) = a(j)+1i*a(j+1);
    n = n +1;
end
b = fft(c)

%% Hex magi
clear
% fid = fopen('b4fft.txt');
% A = textscan(fid,'%s %s %s %s %s %s %s %s','HeaderLines',1);
% A{1} = strcat(A{1},A{2});
% A{2} = strcat(A{3},A{4});
% A{3} = strcat(A{5},A{6});
% A{4} = strcat(A{7},A{8});
% B = [cell2mat(A{1}) ; cell2mat(A{2}) ; cell2mat(A{3}) ; cell2mat(A{4})];
% B = strcat('0x',B,'s32');
% B = hex2dec(B);

% t = 1:numel(B);
% plot(t(1:2:end),B(1:2:end));
%plot(1:1024,B)

filename = 'compareSignal.csv';
FID = fopen(filename);
A = textscan(FID,'%s','Delimiter',',');
compareSignal = A{1,1};
compareSignal = strcat(compareSignal,'s16');
compareSignal = hex2dec(compareSignal);
%compareSignal = [ zeros(431, 1) ; compareSignal ; zeros(401, 1) ];

fclose('all');

filename = 'inSignal.csv';
FID = fopen(filename);
A = textscan(FID,'%s','Delimiter',',');
inSignal = A{1,1};
inSignal = strcat(inSignal,'s16');
inSignal = hex2dec(inSignal);

t = 1:numel(inSignal);
plot(t(1:end),8*inSignal(1:end),t(241:240+length(compareSignal)),compareSignal(1:end));
title('Cross-correlation');
xlabel('Samples');
ylabel('Amplitude (Signed 16-bit)');
legend('inSignal','compareSignal')
xlim([0 1024])
%plot((1:1024),fft(inSignal))


fclose('all');


