%% Floating versus Fixed
% https://ww2.mathworks.cn/help/dsp/ug/floating-point-to-fixed-point-conversion-of-iir-filters.html
% https://se.mathworks.com/help/dsp/ref/fdesign.bandpass.html
clear;
antalBits = 16;
bandpassSpecs = fdesign.bandpass('Fst1,Fp1,Fp2,Fst2,Ast1,Ap,Ast2',.49,.5,.6,.7,60,1,80);
bpFilter = design(bandpassSpecs,'cheby2','SystemObject',true);
bitFilter = design(bandpassSpecs,'cheby2','SystemObject',true);

bitFilter.SOSMatrix = fi(bitFilter.SOSMatrix,1,antalBits)

fvt_comp = fvtool(bpFilter,bitFilter);
legend(fvt_comp,'Floating-point (double) SOS',sprintf('Fixed-point (%d-bit) SOS',antalBits));


%% Ny section
clear;
antalBits = 8;
bandpassSpecs = fdesign.lowpass('Fp,Fst,Ap,Ast',0.1253623,0.1352342,0.1,60)
bpFilter = design(bandpassSpecs,'cheby2','SystemObject',true);
bitFilter = design(bandpassSpecs,'cheby2','SystemObject',true);

bitFilter.SOSMatrix = fi(bitFilter.SOSMatrix,1,antalBits)
bitFilter.ScaleValues = fi(bitFilter.ScaleValues,1,antalBits)

fvt_comp = fvtool(bpFilter,bitFilter);
legend(fvt_comp,'Floating-point (double) SOS',sprintf('Fixed-point (%d-bit) SOS',antalBits));


%% FIR Denne er i rapporten

antalBits = 8;
antalBits2 = 16;
bandpassSpecs = fdesign.lowpass('Fp,Fst,Ap,Ast',0.1253623,0.1352342,0.1,60)
lpKaiser = design(bandpassSpecs,'kaiserwin','SystemObject',true);
firTest = dsp.FIRFilter(double(fi(lpKaiser.Numerator, 1,antalBits)));
firTest2 = dsp.FIRFilter(double(fi(lpKaiser.Numerator, 1,antalBits2)));

fvt_comp = fvtool(lpKaiser, firTest, firTest2);

legend(fvt_comp,'Floating-point (double)',sprintf('Fixed-point (%d-bit)',antalBits),sprintf('Fixed-point (%d-bit)',antalBits2));
