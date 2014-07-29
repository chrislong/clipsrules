package CLIPSJNI;

import static org.junit.Assert.*;

import org.hamcrest.core.IsInstanceOf;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

public class ArgumentsTest {

    private Environment environment;

    @Before
    public void setUp() throws Exception {
        environment = new Environment();
    }

    @Test
    public void testAssert() {
        FactAddressValue factAddress = environment.assertString("(answer 42)");
        Assert.assertNotNull("failed to assert a fact", factAddress);
    }

    @Test
    public void testNew() throws Exception {
        PrimitiveValue value = environment.eval("(new java java.lang.String)");
        assertThat(value.getValue(), new IsInstanceOf(String.class));
    }
    
    @Test
    public void testInteger() throws Exception {
        boolean buildOk = environment.build("(defglobal ?*x* = (new java java.lang.StringBuilder))");
        Assert.assertTrue("defglobal failed", buildOk);
        PrimitiveValue orig = environment.eval("?*x*");
        System.out.println("x (before) =\t'" + orig.getValue() + "'");
        PrimitiveValue eval = environment.eval("(call ?*x* append 42)");
        System.out.println("x type: " + eval.getValue().getClass().getName());
        System.out.println("x (direct) =\t'" + eval.getValue() + "'");
        System.out.println("x =\t'" + environment.eval("(call ?*x* toString)") + "'");
        Assert.assertNotNull("append failed", eval);
        PrimitiveValue lengthVal = environment.eval("(call ?*x* length)");
        Assert.assertEquals("wrong length", 2, lengthVal.intValue());
    }
    
    @Test
    public void testFloat() throws Exception {
        boolean buildOk = environment.build("(defglobal ?*list* = (new java java.util.ArrayList))");
        Assert.assertTrue("defglobal failed", buildOk);
        PrimitiveValue initLength = environment.eval("(call ?*list* size)");
        Assert.assertEquals("bad initial length", 0, initLength.intValue());
        
        float value = 3.14f;
        environment.eval("(call ?*list* add " + value + ")");
        PrimitiveValue newLength = environment.eval("(call ?*list* size)");
        Assert.assertEquals("bad new length", 1, newLength.intValue());
        
        PrimitiveValue element = environment.eval("(call ?*list* get 0)");
        Assert.assertEquals("unexpected value from list", value, element.floatValue(), 0.001f);
    }
    
    @Test(expected=NoSuchMethodException.class)
    public void testUnknownMethod() throws Exception {
        boolean buildOk = environment.build("(defglobal ?*x* = (new java java.lang.String))");
        Assert.assertTrue("defglobal failed", buildOk);
        System.out.println("Should see CLIPS error message, but it's ok.");
        environment.eval("(call ?*x* foobarishness)");
    }
    
    @Test(expected=ArrayIndexOutOfBoundsException.class)
    public void testJavaMethodException() throws Exception {
        boolean buildOk = environment.build("(defglobal ?*list* = (new java java.util.ArrayList))");
        Assert.assertTrue("defglobal failed", buildOk);
        System.out.println("Should see CLIPS error message, but it's ok.");
        environment.eval("(call ?*list* get -1)");
    }
    
    @Test
    public void staticTest() throws Exception {
        boolean buildOk = environment.build("(defglobal ?*sysout* = (call java java.lang.System out))");
        Assert.assertTrue("defglobal failed", buildOk);
        environment.eval("(call ?*sysout* println \"Hi!\")");
    }
    
    @Test
    public void testField() throws Exception {
        boolean buildOk = environment.build("(defglobal ?*foo* = (new java " + Foo.class.getName() + "))");
        Assert.assertTrue("defglobal failed", buildOk);
        PrimitiveValue fooValue = environment.eval("?*foo*");
        System.out.println(fooValue);
        Foo foo = (Foo) fooValue.getValue();
        System.out.println(foo.bar);
        foo.bar = 8;
        PrimitiveValue value = environment.eval("(call ?*foo* bar)");
        Assert.assertEquals("field did not get new value", 8, value.intValue());
    }
}

