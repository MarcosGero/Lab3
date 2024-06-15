import java.io.IOException;

class Main{
    public static void main(String[] args) throws IOException {
        String[] argumentos = new String[] {"lab3tic-backend.exe", "1", "woh.jpg"};
        Process proc = new ProcessBuilder(argumentos).start();
        String result = new String(proc.getInputStream().readAllBytes());
        System.out.println(result);
    }
}