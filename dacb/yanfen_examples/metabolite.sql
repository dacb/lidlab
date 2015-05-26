-- create a table 
CREATE TABLE metabolites (
		sample VARCHAR(64),
		metabolite VARCHAR(64),
		concentration FLOAT,
		conc_error FLOAT
);
CREATE INDEX sample ON metabolites (sample);
CREATE INDEX metabolite ON metabolites (metabolite);
CREATE INDEX sample_metabolite ON metabolites (sample, metabolite);

-- load it from the filesystem
.mode tab
.header on
.import metabolite.tab metabolites

-- count the rows
SELECT COUNT(*) FROM metabolites;

-- return just the first row
SELECT * FROM metabolites LIMIT 1;

-- drop the first row
DELETE FROM metabolites WHERE sample = 'sample';

-- count the rows should be one less than earlier
SELECT COUNT(*) FROM metabolites;

-- return just the first row - look it isn't the header
SELECT * FROM metabolites LIMIT 1;

-- find the valine value for all conditions
SELECT * FROM metabolites WHERE metabolite = 'valine';

-- display the above but pretier
SELECT fm42.metabolite, fm42.concentration AS 'fm42 conc', fm47.concentration AS 'fm47 conc'
	FROM metabolites AS fm42
		INNER JOIN metabolites AS fm47
			ON fm42.metabolite = fm47.metabolite
		WHERE fm42.sample = 'FM42' AND fm47.sample = 'FM47'
			AND fm42.metabolite = 'valine'
;

-- find the average valine value for all conditions
SELECT AVG(concentration) FROM metabolites WHERE metabolite = 'valine';

