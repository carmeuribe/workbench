/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 
#include "MathExpressionTest.h"

#include "CaretMathExpression.h"

#include <cmath>

using namespace caret;
using namespace std;

MathExpressionTest::MathExpressionTest(const AString& identifier) : TestInterface(identifier)
{
}

void MathExpressionTest::execute()
{
    CaretMathExpression myExpr("sin(-yip*5)+x^3*(3+2)");
    vector<float> vars(2);
    const float TOLER = 0.000001f;
    const vector<AString>& varNames = myExpr.getVarNames();
    if (varNames.size() != 2) setFailed("incorrect number of variables found");
    float x = 3.75f;
    float yip = -2.75f;
    if (varNames[0] == "x")
    {
        if (varNames[0] != "x" || varNames[1] != "yip") setFailed("found incorrect variable names");
        vars[0] = x;
        vars[1] = yip;
    } else {
        if (varNames[0] != "yip" || varNames[1] != "x") setFailed("found incorrect variable names");
        vars[0] = yip;
        vars[1] = x;
    }
    double testresult = myExpr.evaluate(vars);
    double correctresult = sin(-yip * 5.0) + pow((double)x, 3.0) * (3.0 + 2.0);
    if (!(abs(testresult - correctresult) <= correctresult * TOLER))//trap NaNs
    {
        setFailed("output value incorrect, expected " + AString::number(correctresult) + ", got " + AString::number(testresult));
    }
}
