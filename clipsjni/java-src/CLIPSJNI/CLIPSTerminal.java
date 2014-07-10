package CLIPSJNI;

import java.awt.Dimension;
import java.awt.Font;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;

import javax.swing.BoxLayout;
import javax.swing.InputMap;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JScrollPane;
import javax.swing.KeyStroke;
import javax.swing.SwingUtilities;

class CLIPSTerminal implements ActionListener
  {  
   private JFrame jfrm;
         
   private Environment clips;
   
   private JTextAreaCommandPromptRouter jta;
      
   /***************/
   /* CLIPSTerminal */
   /***************/
   CLIPSTerminal()
     {  
      /*===================================*/
      /* Create a new JFrame container and */
      /* assign a layout manager to it.    */
      /*===================================*/
     
      jfrm = new JFrame("CLIPSTerminal");          
      jfrm.getContentPane().setLayout(new BoxLayout(jfrm.getContentPane(),BoxLayout.Y_AXIS));
    
      /*=================================*/
      /* Give the frame an initial size. */
      /*=================================*/
     
      jfrm.setSize(480,390);  
  
      /*=============================================================*/
      /* Terminate the program when the user closes the application. */
      /*=============================================================*/
     
      jfrm.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);  
 
      /*=============================*/
      /* Create the text field area. */
      /*=============================*/
 
      clips = new Environment();
      try
        { 
         jta = new JTextAreaCommandPromptRouter(clips); 
         jta.getJTextArea().setFont(new Font("Monospaced", Font.PLAIN, 12));
         jta.getJTextArea().setMargin(new Insets(5,5,5,0));
        }
      catch (Exception e)
        { 
         e.printStackTrace();
         return;
        }       
      clips.addRouter(jta);
      
      /*=======================================*/
      /* Put the text area into a scroll pane. */
      /*=======================================*/

      JScrollPane jscrlp = new JScrollPane(jta.getJTextArea());
      jscrlp.setPreferredSize(new Dimension(350,200));
      
      /*========================================*/
      /* Add the scroll pane to the main frame. */
      /*========================================*/
      
      jfrm.getContentPane().add(jscrlp); 
            
      /*=================================================*/
      /* Get KeyStroke for copy/paste keyboard commands. */
      /*=================================================*/

      KeyStroke copy = KeyStroke.getKeyStroke(KeyEvent.VK_C,KeyEvent.CTRL_MASK);
      KeyStroke paste = KeyStroke.getKeyStroke(KeyEvent.VK_V,KeyEvent.CTRL_MASK);

      /*==========================================================*/
      /* Override copy/paste for the JTextAreaCommandPromptRouter */
      /* so that we can later define our own menu accelerators.   */
      /*==========================================================*/

      String actionKey = "none";
      InputMap map = jta.getJTextArea().getInputMap();
      map.put(copy,actionKey);
      map.put(paste,actionKey);

      /*======================*/
      /* Create the menu bar. */
      /*======================*/
      
      JMenuBar jmb = new JMenuBar();
      
      JMenu jmEdit = new JMenu("Edit");
      JMenuItem jmiCopy = new JMenuItem("Copy",KeyEvent.VK_C);
      JMenuItem jmiPaste = new JMenuItem("Paste",KeyEvent.VK_V);
      
      jmiCopy.setAccelerator(copy);
      jmiPaste.setAccelerator(paste);
      
      jmiCopy.addActionListener(this);
      jmiPaste.addActionListener(this);
      
      jmEdit.add(jmiCopy);
      jmEdit.add(jmiPaste);
      jmb.add(jmEdit);
      
      jfrm.setJMenuBar(jmb);
      
      /*====================*/
      /* Display the frame. */
      /*====================*/

      jfrm.pack();
      jfrm.setVisible(true);  
     }  
     
   /*########################*/
   /* ActionListener Methods */
   /*########################*/

   /*******************/
   /* actionPerformed */
   /*******************/  
   public void actionPerformed(
     ActionEvent ae) 
     {
      try
        { onActionPerformed(ae); }
      catch (Exception e)
        { e.printStackTrace(); }
     }

   /*********************/
   /* onActionPerformed */
   /*********************/  
   public void onActionPerformed(
     ActionEvent ae) throws Exception 
     {      
      /*==========================*/
      /* Handle the Clear button. */
      /*==========================*/

      if (ae.getActionCommand().equals("Copy"))  
        { jta.copy(); }
      else if (ae.getActionCommand().equals("Paste"))  
        { jta.paste(); }
     }
     
   /********/
   /* main */
   /********/  
   public static void main(String args[])
     {  
      /*===================================================*/
      /* Create the frame on the event dispatching thread. */
      /*===================================================*/
      
      SwingUtilities.invokeLater(
        new Runnable() 
          {  
           public void run() { new CLIPSTerminal(); }  
          });   
     }  
  }