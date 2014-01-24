#Some of the ideas used below had inspiration from this MySQL Checker script for autoconf:
#https://code.launchpad.net/~hartmut-php/mysql-automacros/devel
#Thanks goes to Hartmut Holzgraefe

AC_INIT(e4b,0.1)
m4_include([ax_check_openssl.m4])
m4_include([ax_lib_expat.m4])
m4_include([ax_check_mysql.m4])

#Initiate possible arguments

echo `uname -a`

AX_CHECK_OPENSSL(,AC_MSG_FAILURE(["No OpenSSL directory found!"]))

#Check for C and C++ Compilers
AC_PROG_CC
AC_PROG_CXX
AC_LANG(C++)

#Static Variables (none should exist by the end)
AC_SUBST(architecture,32)

#Checks what platform we are running on. For now, assume *nix based distributions.
if test `uname` == "Linux"
then
		AC_SUBST(shared,-shared)

else
	#Assume mac for now.
		AC_SUBST(shared,-bundle)

fi

#Check for libexpat
AX_LIB_EXPAT([1.9])
if test "$HAVE_EXPAT" == "no"
then
    AC_MSG_ERROR(["Libexpat is not installed! Please install libexpat"])
fi


#Checks for MySQL installation.
AX_CHECK_MYSQL(yes,yes,,yes)
AX_SOURCE_MYSQL()


#Old code, doesn't work 100% so commenting out for now.
#Check MySQL source version, returns warning if it doesn't match binary
#MYSQL_SOURCE_V=`grep MYSQL_SERVER_VERSION $with_mysql_source/include/mysql_version.h | sed s/\"$//g | sed s/.*\"//g`
#MYSQL_MINOR_SOURCE_V=`echo $MYSQL_SOURCE_V | cut -c 3` 
#MYSQL_MAJOR_SOURCE_V=`echo $MYSQL_SOURCE_V | cut -c 1`
#MYSQL_REV_SOURCE_V=`echo $MYSQL_SOURCE_V | cut -c 5-6`
#if test "$MYSQL_MAJOR_SOURCE_V" != "$MYSQL_MAJOR_V" || test "$MYSQL_MINOR_SOURCE_V" != "$MYSQL_MINOR_V" || test "$MYSQL_REV_SOURCE_V" != "$MYSQL_REV_V"
#then 
#	AC_MSG_WARN("MySQL Source Version Does not Match MySQL Binary version!")
#	AC_MSG_WARN("Source Version is $MYSQL_SOURCE_V. Binary Version is $MYSQL_V")
#fi



ln -sf $MYSQL_SOURCE/sql sql
ln -sf $MYSQL_SOURCE/regex regex

mkdir -p include
cp -r $MYSQL_SOURCE/include/* include
cp -r $MYSQL_INCLUDES/* include

AC_OUTPUT(Makefile protXML/Makefile)
ac_config_files=" "
AC_CONFIG_FILES(fasta/Makefile fasta/ha_fasta.cc fasta/ha_fasta.h)
AC_SUBST(engine,fasta)
AC_SUBST(ENGINE,FASTA)
AC_OUTPUT()
ac_config_files=" "
AC_CONFIG_FILES(fastq/Makefile fastq/ha_fastq.cc fastq/ha_fastq.h)
AC_SUBST(engine,fastq)
AC_SUBST(ENGINE,FASTQ)
AC_OUTPUT()
ac_config_files=" "
AC_CONFIG_FILES(blast_fmt6/Makefile blast_fmt6/ha_blast_fmt6.cc blast_fmt6/ha_blast_fmt6.h)
AC_SUBST(engine,blast_fmt6)
AC_SUBST(ENGINE,BLAST_FMT6)
AC_OUTPUT()

AC_MSG_NOTICE(Engines for biology has been configured. Please run the make files created by this script:)
AC_MSG_NOTICE($ make)
AC_MSG_NOTICE($ make install)
