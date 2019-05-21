package mainwindow;

import java.io.IOException;
import java.io.*;
import java.util.Scanner;

class Operations {

    void status(SocketHandler s, PrintStream out) throws Exception {
	s.sendMessage("getFishes");
    }

    void addFish(String fishName, String coordinate, String size, String mobility, SocketHandler s, PrintStream out) throws Exception {
	s.sendMessage("addFish " + fishName + " at " + coordinate + "," + size + ", " + mobility);
    }

    void delFish(String fishName, SocketHandler s, PrintStream out) throws Exception {
	s.sendMessage("delFish " + fishName);
	String rec = "";
	try {
	    do {
		rec = s.receiveMessage();
	    } while (rec.startsWith("list") || rec.startsWith("pong"));
	} catch (IOException e) {
	    e.printStackTrace();
	}
	if (rec.equals("OK")) {
	    out.println("OK\n");
	}
	else {
	    out.println(rec + " : Poisson inexistant\n");
	}
    }

    void startFish(String fishName, SocketHandler s, PrintStream out) throws Exception {
	s.sendMessage("startFish " + fishName);
    }
}
