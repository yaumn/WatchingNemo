package mainwindow;

import javafx.application.Application;
import javafx.event.EventHandler;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.layout.Pane;
import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;
import javafx.stage.Stage;
import javafx.scene.control.Button;
import javafx.event.ActionEvent;

import static java.lang.Thread.sleep;

public class Main extends Application {

    @Override
    public void start(Stage primaryStage) throws Exception{
        FXMLLoader loader = new FXMLLoader(getClass().getResource("aquarium.fxml"));
        Parent root = loader.load();
        ClientController cc = loader.getController();
        primaryStage.setTitle("Watching Nemo");
        primaryStage.setScene(new Scene(root));
        primaryStage.setMinHeight(675);
        primaryStage.setMinWidth(800);
        primaryStage.setMaxHeight(675);
        primaryStage.setMaxWidth(800);

        primaryStage.setOnCloseRequest(event -> {
		System.exit(0);
	    });
        primaryStage.show();
        cc.postScene();
    }

    public static void main(String[] args) {
	for (int i = 0; i < args.length; i++) {
	    if (args[i].equals("-i")) {
		ClientController.setId(args[i + 1]);
	    }
	    if (args[i].equals("-l")) {
		ClientController.setLogs(Integer.parseInt(args[i + 1]));
	    }
	}
        launch(args);
    }
}
