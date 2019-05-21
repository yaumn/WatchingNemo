package mainwindow;

import java.io.FileInputStream;
import java.io.IOException;
import java.util.Properties;

import static java.lang.Thread.sleep;

public class MainTest {
    public static void main(String[] args) throws Exception {
        testPing();
    }

    private static void testPing() throws IOException, InterruptedException {
        SocketHandler s = new SocketHandler(1);
        s.startConnection("127.0.0.1", 12345);
        try {
            s.sendMessage("hello");
        } catch (Exception e) {
            e.printStackTrace();
        }
        s.receiveMessage();


        s.startPing(null);
        sleep(10000);
        s.stopConnection();
    }

    public static void testConfig() throws Exception {
        Properties config = new Properties();
        FileInputStream in = new FileInputStream(ClientController.CONFIG_FILE);
        config.load(in);
        in.close();

        System.out.println();
        System.out.println(config.get("controller-port"));
        System.out.println(config.get("display-timeout-value"));
    }
    /*
      public static void testPrompt() throws IOException {
      Prompt p = new Prompt();
      p.read();
      }
    */

}
