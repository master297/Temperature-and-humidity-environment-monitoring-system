import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;


static final String DB_URL = "jdbc:mysql://localhost:3306/iot?useSSL=false&serverTimezone=UTC";
static final String USER = "root";     // 默认通常是 root
static final String PASS = "123456";   // 你的 MySQL 密码

void main() {
    IO.println(">>> Java 服务器启动中...");

    // 1. 监听 8888 端口 (必须和 STM32 代码里的端口一致)
    try (ServerSocket serverSocket = new ServerSocket(8888)) {

        IO.println(">>> 正在等待 STM32 连接 (请确保网络助手已关闭)...");

        // 2. 阻塞等待连接 (程序会停在这里，直到 STM32 连上来)
        Socket socket = serverSocket.accept();
        IO.println(">>> 连接成功！客户端IP: " + socket.getInetAddress());

        // 3. 获取输入流，准备读取数据
        InputStream inputStream = socket.getInputStream();
        BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream));

        String line;
        // 4. 循环按行读取数据
        while ((line = reader.readLine()) != null) {
            // 打印原始数据
            IO.println("收到原始数据: " + line);

            // --- 简单的数据解析演示 ---
            // 假设数据格式固定为: "Temp=25.5, Humi=24.0"
            if (line.startsWith("Temp=")) {
                try {
                    // 简单的字符串处理提取数字
                    String[] parts = line.split(","); // 按逗号分割
                    String tempStr = parts[0].split("=")[1]; // 取 Temp= 后面的数字
                    String humiStr = parts[1].split("=")[1]; // 取 Humi= 后面的数字

                    // 转成浮点数
                    double temperature = Double.parseDouble(tempStr);
                    double humidity = Double.parseDouble(humiStr);

                    IO.println("✅ 解析成功 -> 温度: " + temperature + "℃, 湿度: " + humidity + "%");
                    if(temperature>26){
                        IO.println("达到设定值，风扇开启✅");
                    }else{
                        IO.println("未达到设定值，关闭风扇❌");
                    }
                    // 【未来扩展】在这里把数据存入数据库，或者推送到网页前端
                    saveToDatabase((float) temperature, (float) humidity);

                } catch (Exception e) {
                    IO.println("⚠️ 数据格式解析错误，跳过: " + e.getMessage());
                }
            }
            IO.println("------------------------------------------------");
        }

    } catch (IOException e) {
        e.printStackTrace();
        IO.println("❌ 启动失败！可能是端口被占用（检查网络助手关没关？）");
    }
}

public static void saveToDatabase(float temp, float humi) {
    // SQL 插入语句
    String sql = "INSERT INTO sensor_data (temp, humi) VALUES (?, ?)";

    try (Connection conn = DriverManager.getConnection(DB_URL, USER, PASS);
         PreparedStatement pstmt = conn.prepareStatement(sql)) {

        pstmt.setFloat(1, temp);
        pstmt.setFloat(2, humi);

        int rows = pstmt.executeUpdate(); // 执行插入
        if (rows > 0) {
            IO.println("💾 数据库保存成功！");
        }

    } catch (Exception e) {
        IO.println("❌ 数据库保存失败: " + e.getMessage());
    }
}
