/**
@file  xlfunctions4.cpp
@brief Implementation of Excel callable functions
*/

#include <orflib/market/market.hpp>
#include <orflib/products/europeancallput.hpp>
#include <orflib/products/americancallput.hpp>
#include <orflib/products/barriercallput.hpp>
#include <orflib/methods/pde/pde1dsolver.hpp>

#include <xlorflib/xlutils.hpp>
#include <xlw/xlw.h>

#include <cmath>

using namespace xlw;
using namespace orf;

BEGIN_EXTERN_C

LPXLFOPER EXCEL_EXPORT xlOrfEuroBSPDE(LPXLFOPER xlPayoffType,
	LPXLFOPER xlStrike,
	LPXLFOPER xlTimeToExp,
	LPXLFOPER xlSpot,
	LPXLFOPER xlDiscountCrv,
	LPXLFOPER xlDivYield,
	LPXLFOPER xlVolatility,
	LPXLFOPER xlPdeParams,
	LPXLFOPER xlAllResults)
{
	EXCEL_BEGIN;

	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

	int payoffType = XlfOper(xlPayoffType).AsInt();
	double spot = XlfOper(xlSpot).AsDouble();
	double strike = XlfOper(xlStrike).AsDouble();
	double timeToExp = XlfOper(xlTimeToExp).AsDouble();

	std::string name = xlStripTick(XlfOper(xlDiscountCrv).AsString());
	SPtrYieldCurve spyc = market().yieldCurves().get(name);
	ORF_ASSERT(spyc, "error: yield curve " + name + " not found");

	double divYield = XlfOper(xlDivYield).AsDouble();
	SPtrVolatilityTermStructure spvol;
	if (XlfOper(xlVolatility).IsNumber()) {
		double vol = XlfOper(xlVolatility).AsDouble();
		spvol.reset(new VolatilityTermStructure(&timeToExp, &timeToExp + 1, &vol, &vol + 1));
	}
	else {  // assume string
		std::string volname = xlStripTick(XlfOper(xlVolatility).AsString());
		spvol = market().volatilities().get(volname);
		ORF_ASSERT(spvol, "error: volatility " + volname + " not found");
	}

	// read the PDE parameters
	PdeParams pdeparams = xlOperToPdeParams(xlPdeParams);
	// handle the xlAllResults argument
	bool allresults;
	if (XlfOper(xlAllResults).IsMissing() || XlfOper(xlAllResults).IsNil())
		allresults = false;
	else
		allresults = XlfOper(xlAllResults).AsBool();

	bool setAlignmenttoBarr = false; // force it to be false

	// create the product
	SPtrProduct spprod(new EuropeanCallPut(payoffType, strike, timeToExp));
	// create the PDE solver
	Pde1DResults results;
	Pde1DSolver solver(spprod, spyc, spot, divYield, spvol, results, allresults);
	solver.setAlignment(false);
	solver.solve(pdeparams);

	// write results to the outbound XlfOper
	RW nrows = allresults ? 1 + (RW)results.times.size() : 1;
	COL ncols = allresults ? 2 + (COL)results.values.front().n_rows : 1;
	XlfOper xlRet(nrows, ncols); // construct a range of size nrows x ncols
	if (allresults) {
		xlRet(0, 0) = "Price";
		xlRet(1, 0) = results.prices[0];
		for (RW i = 2; i < nrows; ++i)  xlRet(i, 0) = XlfOper::Error(xlerrNA);

		xlRet(0, 1) = "Time/Spot";
		Vector spots;
		results.getSpotAxis(0, spots);

		for (size_t i = 0; i < spots.size(); ++i)
			xlRet(0, 2 + (COL)i) = spots[i];
		for (size_t i = 0; i < results.times.size(); ++i) {
			xlRet(1 + (RW)i, 1) = results.times[i];
			for (size_t j = 0; j < results.values.front().n_rows; ++j)
				xlRet(1 + (RW)i, 2 + (COL)j) = results.values[i](j, 0);
		}
	}
	else {
		xlRet(0, 0) = results.prices[0];
	}

	return xlRet;

	EXCEL_END;
}

LPXLFOPER EXCEL_EXPORT xlOrfAmerBSPDE(LPXLFOPER xlPayoffType,
	LPXLFOPER xlStrike,
	LPXLFOPER xlTimeToExp,
	LPXLFOPER xlSpot,
	LPXLFOPER xlDiscountCrv,
	LPXLFOPER xlDivYield,
	LPXLFOPER xlVolatility,
	LPXLFOPER xlPdeParams,
	LPXLFOPER xlAllResults)
{
	EXCEL_BEGIN;

	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

	int payoffType = XlfOper(xlPayoffType).AsInt();
	double spot = XlfOper(xlSpot).AsDouble();
	double strike = XlfOper(xlStrike).AsDouble();
	double timeToExp = XlfOper(xlTimeToExp).AsDouble();

	std::string name = xlStripTick(XlfOper(xlDiscountCrv).AsString());
	SPtrYieldCurve spyc = market().yieldCurves().get(name);
	ORF_ASSERT(spyc, "error: yield curve " + name + " not found");

	double divYield = XlfOper(xlDivYield).AsDouble();
	SPtrVolatilityTermStructure spvol;
	if (XlfOper(xlVolatility).IsNumber())
	{
		double vol = XlfOper(xlVolatility).AsDouble();
		spvol.reset(new VolatilityTermStructure(&timeToExp, &timeToExp + 1, &vol, &vol + 1));
	}
	else {  // assume string
		std::string volname = xlStripTick(XlfOper(xlVolatility).AsString());
		spvol = market().volatilities().get(volname);
		ORF_ASSERT(spvol, "error: volatility " + volname + " not found");
	}

	// read the PDE parameters
	PdeParams pdeparams = xlOperToPdeParams(XlfOper(xlPdeParams));
	// handle the xlAllResults argument
	bool allresults;
	if (XlfOper(xlAllResults).IsMissing() || XlfOper(xlAllResults).IsNil())
		allresults = false;
	else
		allresults = XlfOper(xlAllResults).AsBool();

	// create the product
	SPtrProduct spprod(new AmericanCallPut(payoffType, strike, timeToExp));
	// create the PDE solver
	Pde1DResults results;
	Pde1DSolver solver(spprod, spyc, spot, divYield, spvol, results, allresults);
	solver.setAlignment(false);
	solver.solve(pdeparams);

	// write results to the outbound XlfOper
	RW nrows = allresults ? 1 + (RW)results.times.size() : 1;
	COL ncols = allresults ? 2 + (COL)results.values.front().n_rows : 1;
	XlfOper xlRet(nrows, ncols); // construct a range of size nrows x ncols
	if (allresults) {
		xlRet(0, 0) = "Price";
		xlRet(1, 0) = results.prices[0];
		for (RW i = 2; i < nrows; ++i)  xlRet(i, 0) = XlfOper::Error(xlerrNA);

		xlRet(0, 1) = "Time/Spot";
		Vector spots;
		results.getSpotAxis(0, spots);

		for (size_t i = 0; i < spots.size(); ++i)
			xlRet(0, 2 + (COL)i) = spots[i];
		for (size_t i = 0; i < results.times.size(); ++i) {
			xlRet(1 + (RW)i, 1) = results.times[i];
			for (size_t j = 0; j < results.values.front().n_rows; ++j)
				xlRet(1 + (RW)i, 2 + (COL)j) = results.values[i](j, 0);
		}
	}
	else {
		xlRet(0, 0) = results.prices[0];
	}

	return xlRet;

	EXCEL_END;
}

LPXLFOPER EXCEL_EXPORT xlOrfBarrBSPDE(LPXLFOPER xlPayoffType,
	LPXLFOPER xlStrike,
	LPXLFOPER xlTimeToExp,
	LPXLFOPER xlSpot,
	LPXLFOPER xlBarrier,
	LPXLFOPER xlFreq,
	LPXLFOPER xlBarrType,
	LPXLFOPER xlDiscountCrv,
	LPXLFOPER xlDivYield,
	LPXLFOPER xlVolatility,
	LPXLFOPER xlPdeParams,
	LPXLFOPER xlSetAlignment,
	LPXLFOPER xlAllResults)
{
	EXCEL_BEGIN;

	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

	int payoffType = XlfOper(xlPayoffType).AsInt();
	double spot = XlfOper(xlSpot).AsDouble();
	double strike = XlfOper(xlStrike).AsDouble();
	double barrier = XlfOper(xlBarrier).AsDouble();
	double timeToExp = XlfOper(xlTimeToExp).AsDouble();

	// Read in the monthly (0), weekly (1) and daily (2) variables
	int freqtype = XlfOper(xlFreq).AsInt();
	BarrierCallPut::Freq frequency;
	switch (freqtype) {
	case 0:
		frequency = BarrierCallPut::Freq::MONTHLY;
		break;
	case 1:
		frequency = BarrierCallPut::Freq::WEEKLY;
		break;
	case 2:
		frequency = BarrierCallPut::Freq::DAILY;
		break;
	default:
		ORF_ASSERT(0, "error: unknown frequency input type");
	}

	// Get barrier type and convert it into a character array
	std::string barrType_string = XlfOper(xlBarrType).AsString();
	ORF_ASSERT(barrType_string.length() == 2, "length of barriertype input must be equal to 2");
	char barrType[2];
	barrType[0] = std::tolower(barrType_string[0]); // Make lowercase
	barrType[1] = std::tolower(barrType_string[1]); // Make lowercase
	int up_or_down;
	if (barrType[0] == 'u') {
		up_or_down = 1;
		ORF_ASSERT(spot < barrier, "BarrierCallPut: barrier cannot be less than the spot for an up option");
	}
	else if (barrType[0] == 'd') {
		up_or_down = 0;
		ORF_ASSERT(spot > barrier, "BarrierCallPut: barrier cannot be more than the spot for a down option");
	}
	else {
		ORF_ASSERT(0, "error: unknown barrier type")
	}


	std::string name = xlStripTick(XlfOper(xlDiscountCrv).AsString()); // Modify this so that we can accept numeric inputs to create a constant discount curve too
	SPtrYieldCurve spyc = market().yieldCurves().get(name);
	ORF_ASSERT(spyc, "error: yield curve " + name + " not found");

	double divYield = XlfOper(xlDivYield).AsDouble();
	SPtrVolatilityTermStructure spvol;
	if (XlfOper(xlVolatility).IsNumber())
	{
		double vol = XlfOper(xlVolatility).AsDouble();
		spvol.reset(new VolatilityTermStructure(&timeToExp, &timeToExp + 1, &vol, &vol + 1));
	}
	else {  // assume string
		std::string volname = xlStripTick(XlfOper(xlVolatility).AsString());
		spvol = market().volatilities().get(volname);
		ORF_ASSERT(spvol, "error: volatility " + volname + " not found");
	}

	// read the PDE parameters
	PdeParams pdeparams = xlOperToPdeParams(XlfOper(xlPdeParams));
	// handle the xlAllResults argument
	bool allresults;
	if (XlfOper(xlAllResults).IsMissing() || XlfOper(xlAllResults).IsNil())
		allresults = false;
	else
		allresults = XlfOper(xlAllResults).AsBool();

	// handle the xlAllResults argument
	bool setAlignmentoBarr;
	if (XlfOper(xlSetAlignment).IsMissing() || XlfOper(xlSetAlignment).IsNil())
		setAlignmentoBarr = false;
	else
		setAlignmentoBarr = XlfOper(xlSetAlignment).AsBool();

	if (barrType[1] == 'o') { // if we want to price an out option
		// create the product
		SPtrProduct spprod(new BarrierCallPut(payoffType, strike, timeToExp, up_or_down, barrier, frequency));
		// create the PDE solver
		Pde1DResults results;
		Pde1DSolver solver(spprod, spyc, spot, divYield, spvol, results, allresults, barrier);
		solver.setAlignment(setAlignmentoBarr);
		solver.solve(pdeparams);

		// write results to the outbound XlfOper
		RW nrows = allresults ? 1 + (RW)results.times.size() : 1;
		COL ncols = allresults ? 2 + (COL)results.values.front().n_rows : 1;
		XlfOper xlRet(nrows, ncols); // construct a range of size nrows x ncols
		if (allresults) {
			xlRet(0, 0) = "Price";
			xlRet(1, 0) = results.prices[0];
			for (RW i = 2; i < nrows; ++i)  xlRet(i, 0) = XlfOper::Error(xlerrNA);

			xlRet(0, 1) = "Time/Spot";
			Vector spots;
			results.getSpotAxis(0, spots);

			for (size_t i = 0; i < spots.size(); ++i)
				xlRet(0, 2 + (COL)i) = spots[i];
			for (size_t i = 0; i < results.times.size(); ++i) {
				xlRet(1 + (RW)i, 1) = results.times[i];
				for (size_t j = 0; j < results.values.front().n_rows; ++j)
					xlRet(1 + (RW)i, 2 + (COL)j) = results.values[i](j, 0);
			}
		}
		else {
			xlRet(0, 0) = results.prices[0]; 
		}
		return xlRet;
	}
	else if (barrType[1] == 'i') {
		// price the european option
		SPtrProduct spprod2(new EuropeanCallPut(payoffType, strike, timeToExp));
		// create the PDE solver
		Pde1DResults results2;
		Pde1DSolver solver2(spprod2, spyc, spot, divYield, spvol, results2, allresults);
		solver2.setAlignment(false);
		solver2.solve(pdeparams);

		//price the out option

		// create the product
		SPtrProduct spprod(new BarrierCallPut(payoffType, strike, timeToExp, up_or_down, barrier, frequency));
		// create the PDE solver
		Pde1DResults results;
		Pde1DSolver solver(spprod, spyc, spot, divYield, spvol, results, allresults, barrier);
		solver.setAlignment(setAlignmentoBarr);
		solver.solve(pdeparams);

		RW nrows = allresults ? 1 + (RW)results.times.size() : 1;
		COL ncols = allresults ? 2 + (COL)results.values.front().n_rows : 1;
		XlfOper xlRet(nrows, ncols); // construct a range of size nrows x ncols
		if (allresults) {
			xlRet(0, 0) = "Price";
			xlRet(1, 0) = results2.prices[0] - results.prices[0]; // european option - knock-out = knock-in
			for (RW i = 2; i < nrows; ++i)  xlRet(i, 0) = XlfOper::Error(xlerrNA);

			xlRet(0, 1) = "Time/Spot";
			Vector spots;
			results.getSpotAxis(0, spots);

			for (size_t i = 0; i < spots.size(); ++i)
				xlRet(0, 2 + (COL)i) = spots[i];
			for (size_t i = 0; i < results.times.size(); ++i) {
				xlRet(1 + (RW)i, 1) = results.times[i];
				for (size_t j = 0; j < results.values.front().n_rows; ++j)
					xlRet(1 + (RW)i, 2 + (COL)j) = results2.values[i](j, 0) - results.values[i](j, 0);
			}
		}
		else {
			double temp_price = results2.prices[0] - results.prices[0]; // european option - knock-out = knock-in
			xlRet(0, 0) = temp_price > 0.0 ? temp_price: 0.0;
		}
		return xlRet;

	}
	else {
		ORF_ASSERT(0, "error: unknown barrier type")
	}

	EXCEL_END;
}


END_EXTERN_C
