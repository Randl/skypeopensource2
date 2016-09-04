cmake_minimum_required(VERSION 3.3)
project(miracl)

set(SOURCE_FILES mraes.c mralloc.c mrarth0.c mrarth1.c mrarth2.c mrarth3.c mrbits.c mrbrick.c mrbuild.c mrcore.c mrcrt.c mrcurve.c mrdouble.c mrebrick.c mrec2m.c mrecn2.c mrfast.c mrflash.c mrflsh1.c mrflsh2.c mrflsh3.c mrflsh4.c mrfrnd.c mrgcd.c mrgcm.c mrgf2m.c mrio1.c mrio2.c mrjack.c mrlucas.c mrmonty.c mrmuldv.c mrpi.c mrpower.c mrprime.c mrrand.c mrround.c mrscrt.c mrshs.c mrshs256.c mrshs512.c mrsmall.c mrsroot.c mrstrong.c mrxgcd.c mrzzn2.c mrzzn2b.c mrzzn3.c )
set(HEADER_FILES miracl.h mirdef.h)

add_library(miracl ${SOURCE_FILES})