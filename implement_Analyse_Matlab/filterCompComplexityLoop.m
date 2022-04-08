clearvars -global
clear
close all

tic
input = (0.01:0.005:0.3);
output = filterComp(input);

plot(2*output(1,:),output(2,:),2*output(1,:),output(3,:),2*output(1,:),output(4,:))
grid
legend('Inverse Chebyshev','Kaiser window', 'Equiripple')
ylim([0 150])
xlabel( "Transition width [% of stopband frequency]");
ylabel( "MAC operations");
title("Interpolation filter with L = 32 ");


toc


function [results] = filterComp(procentSpace)

    ripple_dB = 0.1;
    stop_dB = 60;
    interPFaktor = 32;
    loops = length(procentSpace);
    
    C_results = zeros(1,loops);
    E_results = zeros(1,loops);
    K_results = zeros(1,loops);
    
    
    parfor i = 1:loops
    
        passB = 1/interPFaktor * (1 - procentSpace(i));
        stopB = 1/interPFaktor * (1 + procentSpace(i));
        bandpassSpecs = fdesign.lowpass('Fp,Fst,Ap,Ast',passB,stopB,ripple_dB,stop_dB);
        bpFilter = design(bandpassSpecs,'cheby2','SystemObject',true);
        
        calcData = bpFilter.SOSMatrix;
        calcAddData = bpFilter.SOSMatrix;
        %additions
        chebADD = size(calcAddData,1) * 4 - sum(calcAddData(:) == 0);
        calcData = [reshape(calcData,1,[]) reshape(bpFilter.ScaleValues,1,[])];
        calcData(calcData == 1) = [];
        calcData(calcData == 0) = [];
        chebMult = size(calcData,2);
    
        C_results(i) = chebMult;
       
        
        lpKaiser = design(bandpassSpecs,'kaiserwin','SystemObject',true);
        kaiserAdd = ceil(length(lpKaiser.Numerator)/interPFaktor);
        kaiserMult = kaiserAdd;
    
        K_results(i) = kaiserAdd;
        
        
        
        
        lpEqui = design(bandpassSpecs,'equiripple','SystemObject',true);
        equiAdd = ceil(length(lpEqui.Numerator)/interPFaktor);
        equiMult = equiAdd;
    
        E_results(i) = equiAdd;
     
    
    end
    
    results = [procentSpace ; C_results ; K_results ; E_results];
end