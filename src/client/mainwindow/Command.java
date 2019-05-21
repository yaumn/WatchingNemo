package mainwindow;

import java.io.*;
import java.util.Scanner;

public abstract class Command {

    public Operations op;

    public abstract void execute(SocketHandler s, PrintStream p) throws Exception;
}
