package com.example.eeeeee;
import java.util.HashMap;
import java.util.Map;
public class SensorDataParser {
    public static Map<String, String> parse(String data) throws IllegalArgumentException{
        try{
            //유효성 검사
            if(data == null || data.trim().isEmpty()){
                throw new IllegalArgumentException("Input data is null or empty");
            }

            //분리
            String[] fields=data.split("/");
            if(fields.length !=10)
            {
                throw new IllegalArgumentException("Invalid data format: expected 10 fields, got " + fields.length);
            }

            // 파싱된 데이터를 Map에 저장 (UI 표시용 텍스트로 변환)
            Map<String, String> parsedData = new HashMap<>();

            for (String field : fields) {
                String[] parts = field.split(":");
                if (parts.length != 2) {
                    throw new IllegalArgumentException("Invalid field format: " + field);
                }

                String key = parts[0];
                String value = parts[1].trim();

                // pl1~pl5: 1=점유, 0=공차
                if (key.matches("pl[1-5]")) {
                    int intValue = parseIntValue(key, value);
                    parsedData.put(key, intValue == 1 ? "점유" : "공차");
                }
                // flame: 1=안전, 0=화재발생
                else if (key.equals("flame")) {
                    int intValue = parseIntValue(key, value);
                    parsedData.put(key, intValue == 0 ? "화재경보꺼짐" : "화재발생");
                }
                // Temp, Humi, empty, Main: 문자열 그대로
                else {
                    parsedData.put(key, value);
                }
            }

            return parsedData;
        }catch (Exception e) {
            throw new IllegalArgumentException("Failed to parse data:" + e.getMessage(), e);
        }
    }
    private static int parseIntValue(String key, String value) {
        try {
            int intValue = Integer.parseInt(value);
            // pl1~pl5, flame은 0 또는 1만 허용
            if (key.matches("pl[1-5]|flame") && (intValue != 0 && intValue != 1)) {
                throw new IllegalArgumentException("Value for " + key + " must be 0 or 1, got " + value);
            }
            return intValue;
        } catch (NumberFormatException e) {
            throw new IllegalArgumentException("Invalid integer value for " + key + ": " + value);
        }
    }
}
