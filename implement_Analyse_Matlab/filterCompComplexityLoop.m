clear;
tic
ripple_dB = 0.1;
stop_dB = 60;
interPFaktor = 8;
procentSpace = (0.0125:0.0125:0.3);
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

    
    %firTest = dsp.FIRFilter(double(fi(lpKaiser.Numerator, 1,16)));
    
    
    lpEqui = design(bandpassSpecs,'equiripple','SystemObject',true);
    equiAdd = ceil(length(lpEqui.Numerator)/interPFaktor);
    equiMult = equiAdd;

    E_results(i) = equiAdd;
    
    %lpIFIR = design(bandpassSpecs,'ifir','SystemObject',true);
    %ifirAdd = length(lpIFIR.Stage1.Numerator) + length(lpIFIR.Stage2.Numerator);
    
    %lpCLSFIR = design(bandpassSpecs,'fircls','SystemObject',true);
    
    %lpFlatFIR = design(bandpassSpecs,'maxflat','SystemObject',true);
    
    %bpFir = design(bandpassSpecs,'ALLFIR');
    
    
    %fvt_comp = fvtool(bpFilter,lpKaiser,lpEqui,firTest);
    %legend(fvt_comp,'cheby2','kaiserwin','equiripple','bit-Kaos');
    
 

end

results = [procentSpace ; C_results ; K_results ; E_results];

toc

