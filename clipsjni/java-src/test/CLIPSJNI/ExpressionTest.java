package CLIPSJNI;

import java.beans.Expression;

public class ExpressionTest {

    public float floatFunc(float x) {
        return x * 2;
    }
    
    public short shortFunc(short x) {
        return (short) (x * 2);
    }
    
    public static void main(String[] args) throws Exception {
        ExpressionTest expressionTest = new ExpressionTest();
        //Object[] floatArgs = { new Double(42.0) };
        Object[] shortArgs = { (short) 3 };
        Expression expression = new Expression(expressionTest, "shortFunc", shortArgs  );
        expression.execute();
        System.out.println("Result: " + expression.getValue());
    }

}
