library(ggplot2)
library(reshape2)

eigenexpression_fraction_plot <- function(eigenexpressions, show_inset=TRUE) {
  fractions <- eigenexpressions^2 / sum(eigenexpressions^2)
  print(paste("fraction of expression[1]: ", fractions[1]))
  print(paste("fraction of expression[2]: ", fractions[2]))
  entropy <- -sum(fractions * log(fractions)) / log(14)
  entropy <- round(entropy * 100) / 100
  print(paste("entropy: ", entropy))
  layout( matrix(c(1,2,2,2,1,2,2,2,1,2,2,2,1,1,1,1), 4, 4, byrow = TRUE) )
  barplot(fractions, ylab = 'Eigenexpression Fraction', xlab = "Eigengene", names=seq(1, arrays), main = paste("Shannon entropy = ", entropy))
  if (show_inset)
    barplot(fractions[2:arrays], ylab = 'Eigenexpression Fraction', xlab = "Eigengene", name=seq(2, arrays))
  layout( matrix(c(1), 1, 1, byrow = T) )
}

eigengene_heatmap_plot <- function(eigengenes) {
  eigengenes.m <- melt(t(eigengenes))
  names(eigengenes.m) <- c("arrays", "eigengenes", "value")
  p <- ggplot(eigengenes.m, aes(x=arrays, y=eigengenes)) 
  p + geom_tile(aes(fill=value), colour = "white") + scale_fill_gradient2() + scale_y_reverse()
}

# display the first N eigengenes
eigengene_profile_plot <- function(eigengenes, N_eigengenes_to_plot) {
  eigengenes.m <- melt(t(eigengenes)[,1:N_eigengenes_to_plot])
  names(eigengenes.m) <- c("arrays", "eigengenes", "value")
  ggplot(eigengenes.m, aes(x = arrays, y = value, color = as.character(eigengenes), group = eigengenes)) + geom_point() + geom_line() + scale_x_discrete(breaks=c(1:arrays), labels=names(d)) + ylab("Expression Level") + xlab("") + theme(axis.text.x=element_text(angle=-270)) + scale_color_discrete(name="Eigengene")
}

N_eigengenes_to_plot <- 4

# read data table
d <- read.table(url("http://genome-www.stanford.edu/SVD/htmls/PNAS/Datasets/Select_Elutriation.txt"), sep="\t", skip = 1, header=T);
row.names(d)=d$UID
d <- subset(d, select=-c(UID))

# perform basic operations like svd, entropy and % variance
arrays = dim(d)[2]
correlation <- (as.matrix(t(d)) %*% as.matrix(d)) / (arrays - 1)
e <- eigen(correlation)
eigengenes <- t(-e$vectors)
eigenexpressions <- sqrt((arrays - 1) * e$values)
eigenarrays <- eigengenes %*% as.matrix(t(d))
#eigenarrays <- eigenarrays / eigenexpressions
for (i in 1:arrays)
  eigenarrays[i,] <- eigenarrays[i,] / eigenexpressions[i]
eigenarrays <- t(eigenarrays)
arraycorrelations <- diag(eigenexpressions) %*% eigengenes
genecorrelations <- eigenarrays %*% diag(eigenexpressions)
genecorrelations <- t(genecorrelations)

# plots
eigenexpression_fraction_plot(eigenexpressions)
eigengene_heatmap_plot(eigengenes);
eigengene_profile_plot(eigengenes, N_eigengenes_to_plot)

# remove first eigenexpression component
eigenexpressions[1] <- 0;
lv <- eigenarrays %*% diag(eigenexpressions) %*% eigengenes
normalization <- log(lv^2)
correlation <- (t(normalization) %*% normalization) / (arrays - 1)
e <- eigen(correlation)
eigenexpressions <- sqrt((arrays - 1) * e$values)
eigengenes = t(e$vectors)
##### WHY!?!?! - row 2?
eigengenes[2,] <- -eigengenes[2,]
eigenarrays <- eigengenes %*% t(normalization)
for (i in 1:arrays)
  eigenarrays[i,] <- eigenarrays[i,] / eigenexpressions[i]
eigenarrays <- t(eigenarrays)
eigenexpression_fraction_plot(eigenexpressions)
eigengene_heatmap_plot(eigengenes);
eigengene_profile_plot(eigengenes, N_eigengenes_to_plot)

# second iteration
print("!!!!not complete!!!!!")

eigenexpressions[1] <- 0;
lv2 <- eigenarrays %*% diag(eigenexpressions) %*% eigengenes
normalization <- sqrt(exp(lv2))
lvs <- sign(lv)
lvd <- lvs * normalization
correlation <- (t(lvd) %*% lvd) / (arrays - 1)
e <- eigen(correlation)
eigenexpressions <- sqrt((arrays - 1) * e$values)
eigengenes = t(e$vectors)
##### WHY!?!?! - row 1?
eigengenes[1,] <- -eigengenes[1,]
eigenarrays <- eigengenes %*% t(normalization)
for (i in 1:arrays)
  eigenarrays[i,] <- eigenarrays[i,] / eigenexpressions[i]
eigenarrays <- t(eigenarrays)
eigenexpression_fraction_plot(eigenexpressions, show_inset=FALSE)
eigengene_heatmap_plot(eigengenes);
eigengene_profile_plot(eigengenes, 2)


print("done")
