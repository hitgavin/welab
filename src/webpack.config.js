// webpack.config.js
const path = require('path');

module.exports = {
  // 设置入口文件
  entry: 'main.js',

  // 定义输出设置
  output: {
    path: path.resolve(__dirname, 'dist'),
    filename: 'bundle.js', // 输出文件的名称
  },

  module: {
    rules: [
      {
        test: /\.(js|jsx)$/, // 匹配 .js 或 .jsx 后缀的文件
        exclude: /node_modules/, // 排除 node_modules 目录
        use: {
          loader: 'babel-loader', // 指定使用 babel-loader
          options: {
            presets: ['@babel/preset-env', '@babel/preset-react'], // 使用预设
          },
        },
      },
      // 可以添加更多的规则来处理其他类型的文件（例如 CSS、图片等）
    ],
  },

  resolve: {
    extensions: ['.js', '.jsx'], // 解析这些扩展名的文件
  },

  // 开发服务器配置（如果需要）
  devServer: {
    contentBase: path.join(__dirname, 'dist'),
    compress: true,
    port: 9000,
  },
};
