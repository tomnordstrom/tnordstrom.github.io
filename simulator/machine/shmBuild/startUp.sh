./shmBuild -1 lmaxgui.json +0

last=$(./shmBuild -last)
./shmBuild -0 system.json +$last

last=$(./shmBuild -last)
./shmBuild -0 ebmComm.json +$last

last=$(./shmBuild -last)
./shmBuild -0 ebm0.json +$last
last=$(./shmBuild -last)
./shmBuild -0 ebm0io.json +$last

last=$(./shmBuild -last)
./shmBuild -0 ebm1.json +$last
last=$(./shmBuild -last)
./shmBuild -0 ebm1io.json +$last
