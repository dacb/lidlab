library(shiny)
library(reshape) 
library(ggplot2)
library(plyr)
library(XLConnect)
library(ggthemes)
require(methods)

# Define server logic required to draw a histogram
shinyServer(function(input, output) {
  #Functions that take inputs and assign variables certain values based on that input
  
  #Chooses which dataset to plot
  datasetInput <- reactive({
    switch(input$dataset,
           "overnights as inoculum-June 2014-Bioscreen" = c("2014_06_06.xlsx","2014_06_06 Bioscreen growth curves.csv"),
           "overnights as inoculum-Bioscreen" = c("2014_07_22.xlsx","2014_07_22 data.csv"),
           "overnights as inoculum-Tubes" = c("2014_07_22_tubes.xlsx","2014_07_22_tubedata.csv"),
           "24 hr induced cultures as inoculum-Bioscreen" = c("2014_07_24.xlsx", "2014_07_24 more 3k3 fdhfoca.csv"),
           "24 hr induced cultures as inoculum-Tubes" = c("2014_07_24_tubes.xlsx", "2014_07_24_tubedata.csv"))

    
  })

  # Chooses which strains to plot
  # Options: "Technical replicates", "Biological Replicates", "Strain Type"
  plotTypeInput <- reactive({
    switch(input$plotType,
           "Technical replicates" = "techreps",
           "Biological Replicates" = "bioreps",
           "Strain Type"= "noreps")
           
  })

  # Chooses which strains to plot
  # Options: "All", "XFLS and Full Pathway", "Control", "XFLS", "Full Pathway"
  plotStrainsInput <- reactive({
    switch(input$plotStrains,
           "All" = c("C","X", "T"),
           "Control and Full Pathway" = c("C", "T"),
           "XFLS and Full Pathway" = c("X", "T"),
           "Control"= "C",
           "XFLS" = "X",
           "Full Pathway" = "T")

  })
  #Chooses which IPTG concentrations to plot
  plotIPTGInput <- reactive({
    switch(input$plotIPTG,
           "Both" = c(0,0.5),
           "0 mM" = 0,
           "0.5 mM" = 0.5)
    
})

#################################################################################################################################
# Expression that generates a plot The expression is
# wrapped in a call to renderPlot to indicate that:
#
#  1) It is "reactive" and therefore should be automatically
#     re-executed when inputs change
#  2) Its output type is a plot
  plotInput <- reactive(function() {
    dataset <-datasetInput()
    Workbook <- loadWorkbook(paste(pathtoAssay, dataset[1], sep=""))
    
    
    # Import a spreadsheet of data about the wells
    Wells.List = readWorksheet(Workbook, sheet = "Wells.List")
    #View(Wells.List)
    
    Wells.List["Media.Name"] <- paste(Wells.List$Glycerol,"% glycerol","&", Wells.List$Formate,"mM formate", Wells.List$IPTG,"mM IPTG",sep = " ")
    Wells.List["Glycerol.Percent"] <- paste(Wells.List$Glycerol,"% glycerol",sep = " ")
    Wells.List["Formate.mM"] <- paste(Wells.List$Formate,"mM formate",sep = " ")
    
    Wells.List["Replicate"] <- sapply(strsplit(as.character(Wells.List$Strain), ""),
                                      function(x) {
                                        x[2]
                                      }
    )
    Wells.List["Strain.Type"] <- sapply(strsplit(as.character(Wells.List$Strain), ""),
                                        function(x) {
                                          x[1]
                                        }
    )
    #View(Wells.List)
    
    ############################################################################################################################
    
    # Import OD bioscreen data
    Bioscreen.Data <- read.csv(paste(pathtoAssay, dataset[2], sep=""),stringsAsFactors=FALSE)
    #View(Bioscreen.Data)
    #View(Bioscreen.Data)
    # Change the time format from 01:01:01 type format to number of hours by using sapply, which allows you to specify a function that is applied to each element you specify (in this case it is the column OD$Time)
    Bioscreen.Data$hours <- sapply(strsplit(as.character(Bioscreen.Data$Time), ":"),
                                   function(x) {
                                     x <- as.numeric(x)
                                     print(x[1]+x[2]/60+x[3]/60^2)
                                     x[1]+x[2]/60+x[3]/60^2
                                   }
    )
    
    ###############################################################################################################################
    #View(Bioscreen.Data)
    Bioscreen.Data.melt <- melt.data.frame(Bioscreen.Data, id=c("Time", "hours"))
    Bioscreen.Data.melt <- rename(Bioscreen.Data.melt, c("variable" = "Well"))
    Bioscreen.Data.melt <- rename(Bioscreen.Data.melt, c("value" = "OD600"))
    #View(Bioscreen.Data.melt)
    Bioscreen.Data.Wells <- merge(Bioscreen.Data.melt, Wells.List)
    # Subsets the all pulling out everything with a Strain in the strains list
    Bioscreen.Data.Wells <- Bioscreen.Data.Wells[Bioscreen.Data.Wells$Strain.Type %in% Wells.List$Strain.Type, ]
    #View(Bioscreen.Data.Wells)
  
    
    Bioscreen.Data.Wells <- transform(Bioscreen.Data.Wells, Strain.Type=ordered(Strain.Type, levels = c("C","X","T")))
    Bioscreen.Data.Wells <- transform(Bioscreen.Data.Wells, Media.Name=ordered(Media.Name, levels = unique(Wells.List$Media.Name)))
    Bioscreen.Data.Wells <- transform(Bioscreen.Data.Wells, Replicate=ordered(Replicate, levels = unique(Wells.List$Replicate)))
    Bioscreen.Data.Wells <- transform(Bioscreen.Data.Wells, Strain=ordered(Strain, levels = unique(Wells.List$Strain)))
    
    Bioscreen.Data.Wells["Strain"] = factor(Bioscreen.Data.Wells$Strain, exclude = NA)
    Bioscreen.Data.Wells["IPTG"] = factor(Bioscreen.Data.Wells$IPTG, exclude = NA)
    plotType <- plotTypeInput()
    plotStrains <- plotStrainsInput()
    plotIPTG <-plotIPTGInput()
    data <- Bioscreen.Data.Wells[Bioscreen.Data.Wells$Strain.Type %in% plotStrains, ]
    data <-data[data$IPTG %in% plotIPTG,]
    #Calculates means and Stdev and  plots them for tech replicates
    Stats <- ddply(data, .(Strain, Time, hours, Media.Name, Glycerol, Glycerol.Percent, Formate.mM, Formate, Strain.Type, IPTG), summarize,
                     mean = round(mean(OD600), 10),
                     sd = round(sd(OD600), 10),
                     N    = length(OD600),
                     se   = round(sd / sqrt(N), 10))
    switch(plotType,
      "techreps" = {
        plot.replicates <- ggplot(data=data, aes(x=hours, y=OD600, group = Well, colour=Strain , shape = as.factor(IPTG))) + geom_line() + geom_point() + xlab("hours") + ylab("OD600") + ggtitle("Glycerol [%] vs. Formate [mM]") + theme_bw()+ theme(axis.text.x  = element_text(size=11), legend.position = "bottom")+ ylim(input$deltaOD[1],input$deltaOD[2])+xlim(input$deltaTime[1],input$deltaTime[2])
        plot.replicates.faceted <- plot.replicates +facet_grid(Glycerol.Percent ~ Formate.mM)
      },
      bioreps = {
        plot.replicates <- ggplot(data=Stats, aes(x=hours, y=mean, group = Strain, colour=Strain.Type, shape= as.factor(IPTG))) + geom_line() + geom_point() + xlab("hours") + ylab("OD600") + ggtitle(paste("Glycerol [%] vs. Formate [mM]", sep= "")) + theme_bw()+ theme(axis.text.x  = element_text(size=11), legend.position = "bottom")+ ylim(input$deltaOD[1],input$deltaOD[2])+xlim(input$deltaTime[1],input$deltaTime[2])
        if(input$errorBars == TRUE){
          plot.replicates <- plot.replicates + geom_errorbar(aes(ymin=mean-sd, ymax=mean+sd), colour="black", width=.1, position=position_dodge(.1))
        }
        plot.replicates.faceted <- plot.replicates +facet_grid(Glycerol.Percent ~ Formate.mM)
      },
      noreps ={
        plot.replicates <- ggplot(data=Stats, aes(x=hours, y=mean, group = Strain.Type, colour=Strain.Type, shape = as.factor(IPTG))) + geom_line() + geom_point() + xlab("hours") + ylab("OD600") + ggtitle(paste("Glycerol [%] vs. Formate [mM]", sep= "")) + theme_bw()+ theme(axis.text.x  = element_text(size=11), legend.position = "bottom")+ ylim(input$deltaOD[1],input$deltaOD[2])+xlim(input$deltaTime[1],input$deltaTime[2])
        if(input$errorBars == TRUE){
          plot.replicates <- plot.replicates + geom_errorbar(aes(ymin=mean-sd, ymax=mean+sd), colour="black", width=.1, position=position_dodge(.1))
        }
        plot.replicates.faceted <- plot.replicates +facet_grid(Glycerol.Percent ~ Formate.mM)
      },
    {
      }
    )
    
  })
  
  output$growthcurvePlot <- renderPlot({
    print(plotInput())
  }, height = 1000)
  
  
  output$downloadPlot <- downloadHandler(
    filename = function() { 
      print(paste(input$fileName, ".png", sep=''))
      paste(input$fileName, ".png", sep='')
      },  
    content = function(file){
        png(file)
        print(plotInput())
        dev.off()
      #device <- function(..., width, height) grDevices::png(..., width = width, height = height, res = 300, units = "in")
      #ggsave(file = file, plot = plotInput(), device = device,width=12, height = 12)
    }
  )
  
})