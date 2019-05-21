package mainwindow;

import javafx.application.Platform;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.layout.Pane;
import javafx.scene.paint.Color;
import javafx.scene.shape.Rectangle;
import javafx.stage.Stage;
import javafx.scene.image.ImageView;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class DrawFishes {
    static void draw(String info, Pane p)
    {
        Platform.runLater(new Runnable() {
		@Override
		public void run() {
		    p.getChildren().clear();
		    String infoList[] = info.split(" \\[");

		    for (String s : infoList) {
			Pattern pattern = Pattern.compile("(\\w+) at (\\d+)x(\\d+), (\\d+)x(\\d+), (\\d+)\\]");
			Matcher matcher = pattern.matcher(s);

			if (matcher.find()) {
			    if (matcher.groupCount() != 6) {
				continue;
			    }

			    try {
				Fish f = new Fish(matcher.group(1),
						  new Position(Double.parseDouble(matcher.group(4)),
							       Double.parseDouble(matcher.group(5))),
						  new Position(Integer.parseInt(matcher.group(2)),
							       Integer.parseInt(matcher.group(3))));

				f.display(p);
			    } catch (Exception e) {
				e.printStackTrace();
			    }
			}
		    }
		}
	    });
    }
}
