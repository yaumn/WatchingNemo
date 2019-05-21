package mainwindow;

import java.io.*;
import java.util.Scanner;

public class AddFish extends Command{

    private String fishName;
    private String coordinate;
    private String size;
    private String mobility;

    AddFish(String fn, String cd, String s, String m) {
	fishName = fn;
	coordinate = cd;
	size = s;
	mobility = m;
	op = new Operations();
    }

    public void execute(SocketHandler s, PrintStream p) throws Exception {
	op.addFish(fishName, coordinate, size, mobility, s, p);
    }
}
  
