MAN ANDREI VLAD 336 CA


1. MALLOC
    -- prima oara incercam sa merge-uim block-urile FREE daca exista block-uri
    -- separam implementarea cu mmap de cea cu sbrk
    
    a. SBRK
        -- prima oara (la inceput) alocam un threshhold
        pe care il returnam
        -- altfel avem urmatorii pasi
            - cautam un block liber optim
                - daca il gasim, verificam daca se poate face split
                - daca nu, il folosim direct
            - daca nu gasim, incercam sa extindem ultimul block daca este FREE
            - altfel alocam memorie noua
    b. MMAP
        -- returnam payload-ul unui block alocat cu mmap

2. FREE
    -- verificam status-ul block-ului
    -- daca e ALOCAT, setam block-ul ca FREE
    -- daca e MAPAT, dam unmap

3. CALLOC
    -- pentru dim mai mari decat page_size se aloca cu mmap iar apoi memset pe 0
    -- pt dim mai mici se aloca prin malloc si memset pe 0

4. REALLOC
    -- se respecta conditiile date pt ptr/size/status
    -- daca dimensiunea ceruta este mai mica decat ce avem actual
        a. BLOCK-ul curent este MAPPED => Trebuie o noua alocare, memcpy si free

        b. BLOCK-ul curent este ALOCAT => incercam split
    -- daca dimensiunea este mai mare
        -- verificam ce status va avea noul block cerut => MAPPED vs ALLOCATED
        a. MAPPED 
            - trebuie sa alocam memorie, copiam si dam free la cea veche
        b. ALLOCATED
            - se incearca unirea block-urilor pana la size-ul dorit
            - daca avem succes, intoarcem aceeasi adresa
            - daca nu avem succes, incercam extinderea (Daca e ultimul)
            - realocam folosind malloc, memcpy si free