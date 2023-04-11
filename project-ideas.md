# Ideas

Computer on power/ on battery

General purpose/multi platform compilers

Brug hardware til ting de ikke er bygget til

Compare samme algoritme på forskelligt hardware

Cooling efficiency sustained load or economics of cooling

Droppe ud fra 5.

Compare canvas'

Noget med GPUer

GPU accelerated Bong Maps

Lav software i flere sprog, compare

Thread management på forskellige OS's og sprog - affinity

forskellige compilers, forskellig thread count, forskellig affinity, forskellige cpu'er

afprøve om der er forskel i performance ved at bruge forskellige compilers
- hvorfor er der forskel? context switches måske?

merge sort hvor man dividerer med n i stedet for 2

måske kan x86 bruge operationer som minps, som gør den bedre end ARM til at lægge flere tal sammen


formål:
hvordan får man den hurtigste merge sort?
måske er singlethreaded bedre en multithreaded for nogle inputstørrelser
måske teste i scenarie hvor input ikke kan være i memory

variable:
- threads
- compiler
- thread affinity
- arkitektur (new x86 vs old x86 (may not have som SIMD instructions) vs arm)
- merge sort arity (måske udnytte specifikke instruktioner til at sammenligne flere tal)

performance indicators:
- undersøge assembly
- context switches (måske lavere ved fixed affinity)
- antal instruktioner udført



