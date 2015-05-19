library(XLConnect)
library(plyr)
library(reshape)
library(ggplot2)
library(ggthemes)

#Sets the working directory correctly for a specific computer
workingDir <- getwd()
m<- regexpr("C:/Users/.*?/",workingDir)
setwd(paste(regmatches(workingDir, m), "Skydrive", sep = ""))
newWorkingDir <- getwd()

#Loads workbook
A <- loadWorkbook(paste(newWorkingDir,"/Assays/Growth Curves/2014_08_Formate_Growth/five reps_formate dependent growth.xlsx", sep=""))
raw <- readWorksheet(A, "data")
#View(raw)

#Reformats the data into one line per measurement
data.melt <- melt.data.frame(raw, id=c("Strain.Type", "Replicate", "Formate_mM", "Strain"))
data.melt$Time_hr <-sapply(data.melt$variable, function(x) {gsub("[^0-9]", "", x)})
data.melt$variable <- NULL
data.melt <- rename(data.melt, c("value" = "OD600"))
data.melt$OD600 <- as.numeric(data.melt$OD600)
data.melt$Time_hr<- as.numeric(data.melt$Time_hr)
#View(data.melt)

#Plots the data
plot.OD600 <-ggplot(data=data.melt, aes(x=Time_hr, y=OD600, group = Strain, shape = Strain.Type, colour = Strain.Type)) + geom_line(size=1) + geom_point(size = 3) + xlab("hours") + ylab("OD600") + ggtitle("Growth Curves") + theme_bw()+ theme(axis.text.x  = element_text(size=11))

plot.OD600.faceted  <- plot.OD600 + facet_grid( ~ Formate_mM, scales ="free")+ theme(strip.text.x = element_text(size=10, angle=0))
plot.OD600.faceted
ggsave(plot.iso.faceted,file=paste(newWorkingDir, "/Assays/Growth Curves/2014_08_Formate_Growth/", "plots_2014_05_14/pTrc_single_strain_DHAP3pgM3wrap_rep.svg", sep=""), width=12, height = 12)


## Combines into plot with means and stdev

Stats <- ddply(data.melt, .(Strain.Type, Time_hr, Formate_mM), summarize,
               mean = round(mean(OD600), 10),
               sd = round(sd(OD600), 10),
               N    = length(OD600),
               se   = sd / sqrt(N) )

plot.stats <- ggplot(data=Stats, aes(x=Time_hr, y=mean, group = Strain.Type, colour = Strain.Type)) + geom_line(size=1)+geom_point(size =3) +xlab("hours")+ ylab("OD600")+ggtitle("Growth Curves-Tubes N=5")+theme_bw()+geom_errorbar(aes(ymin=mean-se, ymax=mean+se), colour="black", width=.3, position=position_dodge(.1))

plot.stats.faceted <-plot.stats +facet_grid(~ Formate_mM)
plot.stats.faceted

