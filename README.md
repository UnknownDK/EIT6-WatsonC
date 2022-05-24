# EIT6-640

This GitHub repo contains code related to EIT6-640s bachelor project.

### The following file tree is an outline of the important files in this GitHub project

```
/
└── aic_workspace/
    └── P6_Acoustic_water_flow_meas/
        ├── main.c
        ├── main.h
        ├── src/
        │   ├── CrossCorrelation/
        │   │   └── cross_corr.c
        │   └── Flowmeter/
        │       ├── aic3204_init.c
        │       ├── circular_dma_reader.c
        │       ├── expansion_board.c
        │       ├── pulse_detector.c
        │       ├── pulse_generator.c
        │       ├── pulse_refine.c
        │       ├── sing_around.c
        │       └── stopwatch.c
        └── include/
            ├── CrossCorrelation/
            │   └── cross_corr.h
            └── Flowmeter/
                ├── aic3204_init.h
                ├── circular_dma_reader.h
                ├── expansion_board.h
                ├── pulse_detector.h
                ├── pulse_generator.h
                ├── pulse_refine.h
                ├── sing_around.h
                └── stopwatch.h
```

The rest of the files are Matlab scripts for plotting, helper scripts for e.g. extracting data from memory of the ezDSP, PCB files, LTSpice simulations and libraries for the ezDSP.