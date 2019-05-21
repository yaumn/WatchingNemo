package mainwindow;

import javafx.application.Platform;
import javafx.scene.control.TextInputControl;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PipedInputStream;
import java.io.PipedOutputStream;
import java.nio.ByteBuffer;
import java.nio.CharBuffer;
import java.nio.charset.Charset;
import java.nio.charset.CharsetDecoder;

class TextInputControlStream {

    private final TextInputControlInputStream in;
    private final TextInputControlOutputStream out;
    private final Charset charset;

    TextInputControlStream(final TextInputControl textInputControl, Charset charset) {
        this.charset = charset;
        this.in = new TextInputControlInputStream(textInputControl);
        this.out = new TextInputControlOutputStream(textInputControl);

        textInputControl.addEventFilter(KeyEvent.KEY_PRESSED, e -> {
		if (e.getCode() == KeyCode.ENTER) {
		    getIn().enterKeyPressed();
		    return;
		}

		if (textInputControl.getCaretPosition() <= getIn().getLastLineBreakIndex()) {
		    e.consume();
		}
	    });
        textInputControl.addEventFilter(KeyEvent.KEY_TYPED, e -> {
		if (textInputControl.getCaretPosition() < getIn().getLastLineBreakIndex()) {
		    e.consume();
		}
	    });
    }

    void clear() throws IOException {
        this.in.clear();
        this.out.clear();
    }

    TextInputControlInputStream getIn() {
        return this.in;
    }

    TextInputControlOutputStream getOut() {
        return this.out;
    }

    private void startProgramInput() {
        // do nothing
    }

    private void endProgramInput() {
        getIn().moveLineStartToEnd();
    }

    private Charset getCharset() {
        return this.charset;
    }

    class TextInputControlInputStream extends InputStream {

        private final TextInputControl textInputControl;
        private final PipedInputStream outputTextSource;
        private final PipedOutputStream inputTextTarget;
        private int lastLineBreakIndex = 0;

        TextInputControlInputStream(TextInputControl textInputControl) {
            this.textInputControl = textInputControl;
            this.inputTextTarget = new PipedOutputStream();
            try {
                this.outputTextSource = new PipedInputStream(this.inputTextTarget);
            } catch (IOException e1) {
                throw new RuntimeException(e1);
            }
        }

        int getLastLineBreakIndex() {
            return this.lastLineBreakIndex;
        }

        void moveLineStartToEnd() {
            this.lastLineBreakIndex = this.textInputControl.getLength();
        }

        void enterKeyPressed() {
            synchronized (this) {
                try {
                    this.textInputControl.positionCaret(this.textInputControl.getLength());

                    final String lastLine = getLastLine();
                    final ByteBuffer buf = getCharset().encode(lastLine + "\r\n"); //$NON-NLS-1$
                    this.inputTextTarget.write(buf.array(), 0, buf.remaining());
                    this.inputTextTarget.flush();
                    this.lastLineBreakIndex = this.textInputControl.getLength() + 1;
                } catch (IOException e) {
                    if ("Read end dead".equals(e.getMessage())) {
                        return;
                    }
                    throw new RuntimeException(e);
                }
            }
        }

        private String getLastLine() {
            synchronized (this) {
                return this.textInputControl.getText(this.lastLineBreakIndex, this.textInputControl.getLength());
            }
        }

        @Override
        public int available() throws IOException {
            return this.outputTextSource.available();
        }

        @Override
        public int read() {
            try {
                return this.outputTextSource.read();
            } catch (IOException ex) {
                return -1;
            }
        }

        @Override
        public int read(final byte[] b, final int off, final int len) {
            try {
                return this.outputTextSource.read(b, off, len);
            } catch (IOException ex) {
                return -1;
            }
        }

        @Override
        public int read(final byte[] b) {
            try {
                return this.outputTextSource.read(b);
            } catch (IOException ex) {
                return -1;
            }
        }

        @Override
        public void close() throws IOException {
            super.close();
        }

        void clear() throws IOException {
            this.inputTextTarget.flush();
            this.lastLineBreakIndex = 0;
        }
    }

    final class TextInputControlOutputStream extends OutputStream {

        private final TextInputControl textInputControl;
        private final CharsetDecoder decoder;
        private ByteArrayOutputStream buf;

        TextInputControlOutputStream(TextInputControl textInputControl) {
            this.textInputControl = textInputControl;
            this.decoder = getCharset().newDecoder();
        }

        @Override
        public synchronized void write(int b) {
            synchronized (this) {
                if (this.buf == null) {
                    this.buf = new ByteArrayOutputStream();
                }
                this.buf.write(b);
            }
        }

        @Override
        public void flush() {
            Platform.runLater(() -> {
		    try {
			flushImpl();
		    } catch (IOException e) {
			throw new RuntimeException(e);
		    }
		});
        }

        private void flushImpl() throws IOException {
            synchronized (this) {
                if (this.buf == null) {
                    return;
                }
                startProgramInput();
                final ByteBuffer byteBuffer = ByteBuffer.wrap(this.buf.toByteArray());
                final CharBuffer charBuffer = this.decoder.decode(byteBuffer);
                try {
                    this.textInputControl.appendText(charBuffer.toString());
                    this.textInputControl.positionCaret(this.textInputControl.getLength());
                } finally {
                    this.buf = null;
                    endProgramInput();
                }
            }
        }

        @Override
        public void close() throws IOException {
            flush();
        }

        void clear() {
            this.buf = null;
        }
    }
}
