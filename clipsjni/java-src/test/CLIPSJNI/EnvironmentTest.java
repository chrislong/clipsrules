package CLIPSJNI;

import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

public class EnvironmentTest {

	private class PrivateValue {

	}

	private Environment environment;

	@Before
	public void setUp() throws Exception {
		environment = new Environment();
	}

	@Test
	public void testSetGlobal() {
		PrivateValue privateValue = new PrivateValue();
		environment.build("(defglobal ?*theAnswer* = 0)");
		environment.setGlobal("theAnswer", privateValue);
		PrimitiveValue retrievedValue = environment.eval("?*theAnswer*");
		Assert.assertSame(privateValue, retrievedValue.getValue());
	}

}
