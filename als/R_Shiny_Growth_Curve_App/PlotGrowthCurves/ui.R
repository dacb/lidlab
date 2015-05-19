library(shiny)

# Define UI for application that draws a histogram
shinyUI(fluidPage(
  
  # Application title
  titlePanel("Growth Curve Plot"),
  
  # Sidebar with a slider input time range, select box for plot type
  sidebarLayout(
    sidebarPanel(
      #Select what you want to plot
      selectInput("dataset", "Choose what dataset you want to plot:", 
                  choices = c("overnights as inoculum-June 2014-Bioscreen","overnights as inoculum-Bioscreen", "overnights as inoculum-Tubes", "24 hr induced cultures as inoculum-Bioscreen", "24 hr induced cultures as inoculum-Tubes")),
      #Select a plot type
      selectInput("plotType", "Choose what you want to plot:", 
                  choices = c("Technical replicates", "Biological Replicates", "Strain Type")),
      #Select strains to plot
      selectInput("plotStrains", "Which strains to plot:", 
                  choices = c("All", "Control and Full Pathway","XFLS and Full Pathway", "Control", "XFLS", "Full Pathway")),
      #Select IPTG concentration(s) to plot
      selectInput("plotIPTG", "Which IPTG concentration(s) to plot:", 
                  choices = c("Both", "0 mM","0.5 mM")),
      
      #Specifies a time range to view
      sliderInput("deltaTime",
                  "Time Range to View:",
                  min = 0,
                  max = 500,
                  value = c(0,200)),
      #Specifies a OD600 range to view
      sliderInput("deltaOD",
                  "OD600 Range to View:",
                  min = 0,
                  max = 1.5,
                  value = c(0.2,0.4)),
      # Show error bars
      checkboxInput("errorBars", "Show Error Bars", FALSE),
      submitButton("Update Plot"),
      
      #Filename input
      textInput("fileName", "Filename:", "2014_8_07 growth curve"),
      
      #Save image button
      downloadButton('downloadPlot', 'Download Plot')
    ),
    
    # Show a plot of the generated distribution
    mainPanel(
      plotOutput("growthcurvePlot", width = "100%")
    )
  )
))