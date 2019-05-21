package mainwindow;

import java.io.*;
import java.util.Scanner;

public class Status extends Command{

    Status() {
	op = new Operations();
    }

    public void execute(SocketHandler s, PrintStream p) throws Exception {
	op.status(s, p);
    }
}
