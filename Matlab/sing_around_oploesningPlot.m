%% d 
clear
l = 1.4142;%1.4142   ; %laengde paa soundpath
angle = 45 *pi / 180; %vinkel mellem sensorer
sampleFreq = 100e3;
c = 1500; %speed of 
timeRes = 1/sampleFreq;
i = 1;
n = 1 %antal sing around loops

v = 1;

t1 = l / (c+v*cos(angle));
f1 = 1 / t1;
t2 = l / (c-v*cos(angle));
f2 = 1 / t2;
deltaF = f1-f2;
test = abs(t2 - t1)

speed = (deltaF*l^2)/(2*l*cos(angle))

velocityRes(i) = (l/2)*((timeRes/n)/(t1*t2));
sampleTid(i) = n * (t1 + t2);
i = i + 1;



%%
clear
l_1 = 1   ; %laengde paa soundpath
angle1 = 45 *pi / 180; %vinkel mellem sensorer
sampleFreq_1 = 100e3;
speed_1 = 0.3;
[sampleTid1, velocityRes1] = singAround(l_1,angle1,sampleFreq_1, speed_1);


subplot(2,2,1)
semilogy(sampleTid1, velocityRes1)
grid
ylim([10e-5 10])
xlim([0 5])
title('Sample frequency = 100 kSPS, L = 1 m')
ylabel('Velocity resolution [m/s]')
xlabel('Time [s]')

l_2 = 5   ; %laengde paa soundpath
angle2 = 45 *pi / 180; %vinkel mellem sensorer
sampleFreq_2 = 100e3;
speed_2 = 0.3;
[sampleTid2, velocityRes2] = singAround(l_2,angle2,sampleFreq_2, speed_2);

subplot(2,2,2)
semilogy(sampleTid2, velocityRes2)
grid
ylim([10e-5 10])
xlim([0 5])
title('Sample frequency = 100 kSPS, L = 5 m')
ylabel('Velocity resolution [m/s]')
xlabel('Time [s]')


l_3 = 1   ; %laengde paa soundpath
angle3 = 45 *pi / 180; %vinkel mellem sensorer
sampleFreq_3 = 500e3;
speed_3 = 0.3;
[sampleTid3, velocityRes3] = singAround(l_3,angle3,sampleFreq_3, speed_3);

subplot(2,2,3)
semilogy(sampleTid3, velocityRes3)
ylim([10e-5 10])
xlim([0 5])
grid
title('Sample frequency = 500 kSPS, L = 1 m')
ylabel('Velocity resolution [m/s]')
xlabel('Time [s]')


l_4 = 5   ; %laengde paa soundpath
angle4 = 45 *pi / 180; %vinkel mellem sensorer
sampleFreq_4 = 500e3;
speed_4 = 0.3;
[sampleTid4, velocityRes4] = singAround(l_4,angle4,sampleFreq_4, speed_4);

subplot(2,2,4)
semilogy(sampleTid4, velocityRes4)
grid
ylim([10e-5 10])
xlim([0 5])
title('Sample frequency = 500 kSPS, L = 5 m')
ylabel('Velocity resolution [m/s]')
xlabel('Time [s]')


sgtitle('Velocity resolution as a function of sample time') 
set(gcf, 'Position',  [100, 50, 900, 670])




function [time, vel] = singAround(crossLength, angle, sample_f, speed)
    
    i = 1;
    v = speed;
    c = 1500; %speed of water
    timeRes = 1/sample_f;
    t1 = crossLength / (c+v*cos(angle));
    t2 = crossLength / (c-v*cos(angle));
    for n = 1:1:5000 %antal sing around loops        
        
        %f1 = 1 / t1;
        
        %f2 = 1 / t2;

        %deltaF = f1-f2;
        %speedTjek = (deltaF*l^2)/(2*l*cos(angle));
        
        vel(i) = (crossLength/2)*((timeRes/n)/(t1*t2));
        time(i) = n * (t1 + t2);
        i = i + 1;
    end

end 


