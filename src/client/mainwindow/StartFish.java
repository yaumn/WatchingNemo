package mainwindow;

import java.io.*;
import java.util.Scanner;

public class StartFish extends Command{

  private String fishName;

  StartFish(String fn) {
    fishName = fn;
    op = new Operations();
  }

  public void execute(SocketHandler s, PrintStream p) throws Exception {
    op.startFish(fishName, s, p);
  }
}
