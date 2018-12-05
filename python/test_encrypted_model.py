# -- coding: utf-8 --
import tensorflow as tf
import tensorflow.examples.tutorials.mnist.input_data as input_data
# from encrypt_the_test_data import encryt_mnist_test, decrypt_and_decode_an_image
# from ops_encrypt.encrypt_conv2d import encrypt_conv2d, gen_cipher_matrix, assgin_cipher_matrix
# from ops_encrypt.encrypt_relu import encrypt_relu_zone1_degree4
# from ops_encrypt.encrypt_avg_pooling import encrypt_avg_pooling
# from ops_encrypt.encrypt_flat import flat
# from ops_encrypt.encrypt_matmul import encrypt_MatMul
# from seal import Plaintext
import time
import numpy as np


def get_weight_bias():
    test_image = input_data.read_data_sets("input_data", one_hot=True).test.images
    x_image = tf.reshape(test_image, [-1, 28, 28, 1])
    y_image = input_data.read_data_sets("input_data", one_hot=True).test.labels

    with tf.Session() as sess:
        x_image = sess.run(x_image)
        x_image = x_image * 255 / 128
        # x_image = np.round(x_image)
        x_image = np.array(x_image, dtype=int)
        x_image = tf.constant(x_image, dtype=tf.float32)
        # load the meta graph and weights
        saver = tf.train.import_meta_graph('model_5/mnist.ckpt-10000.meta')
        saver.restore(sess, tf.train.latest_checkpoint('model_5/'))
        # saver = tf.train.import_meta_graph('model_6\mnist.ckpt-9990.meta')
        # saver.restore(sess, tf.train.latest_checkpoint('model_6/'))

        # get weights
        graph = tf.get_default_graph()
        conv1_w = sess.run(graph.get_tensor_by_name('conv1/w:0'))
        conv1_w = np.round(conv1_w * 10)
        conv1_b = sess.run(graph.get_tensor_by_name('conv1/b:0'))
        conv1_b = np.round(conv1_b * 10)
        print(conv1_b.shape)
        conv2_w = sess.run(graph.get_tensor_by_name('conv2/w:0'))
        conv2_w = np.round(conv2_w * 10)
        conv2_b = sess.run(graph.get_tensor_by_name('conv2/b:0'))
        conv2_b = np.round(conv2_b * 10)

        fc1_w = sess.run(graph.get_tensor_by_name('fc1/w:0'))
        fc1_w = np.round(fc1_w * 10)
        fc1_b = sess.run(graph.get_tensor_by_name('fc1/b:0'))
        fc1_b = np.round(fc1_b * 10)

        fc2_w = sess.run(graph.get_tensor_by_name('fc2/w:0'))
        fc2_w = np.round(fc2_w * 10)
        fc2_b = sess.run(graph.get_tensor_by_name('fc2/b:0'))
        fc2_b = np.round(fc2_b * 10)

        run_raw(x_image, conv1_w, conv1_b, conv2_w, conv2_b, fc1_w, fc1_b, fc2_w, fc2_b, y_image, sess)
        # run_raw_new_relu(x_image, conv1_w, conv1_b, conv2_w, conv2_b, fc1_w, fc1_b, fc2_w, fc2_b, y_image, sess)
        # run_encrypt(encrypt_images, conv1_w, conv1_b, conv2_w, conv2_b, fc1_w, fc1_b, fc2_w, fc2_b, y_image, sess)


def run_raw(x_image, conv1_w, conv1_b, conv2_w, conv2_b, fc1_w, fc1_b, fc2_w, fc2_b, y_image, sess):
    spaced_text = "test raw dataset"
    print(spaced_text.center(72, "#"))
    output1 = tf.nn.conv2d(x_image, conv1_w, strides=[1, 2, 2, 1], padding='VALID', use_cudnn_on_gpu=None) + conv1_b
    # output1 = sess.run(output1)
    # f = open("conv1_result.txt", 'w+')
    # for index_image in range(10):
    #     for index_channel in range(32):
    #         for index_i in range(28):
    #             for index_j in range(28):
    #                 f.write(str(output1[index_image][index_i][index_j][index_channel]) + ',')
    #             f.write('\n')
    # f.close()
    output1 = sess.run(output1)
    tmp = output1[0, :, :, 0]
    output1 = square(output1)
    tmp = output1[0, :, :, 0]
    output1 = tf.nn.avg_pool(output1, ksize=[1, 3, 3, 1], strides=[1, 1, 1, 1], padding='SAME')
    output1 = sess.run(output1)*9
    tmp = output1[0, :, :, 0]

    output2 = tf.nn.conv2d(output1, conv2_w, strides=[1, 2, 2, 1], padding='VALID', use_cudnn_on_gpu=None) + conv2_b
    output2 = sess.run(output2)
    tmp = output2[0, :, :, 0]
    output2 = tf.nn.avg_pool(output2, ksize=[1, 3, 3, 1], strides=[1, 1, 1, 1], padding='SAME')
    output2 = sess.run(output2)*9
    tmp = output2[0, :, :, 0]

    output2 = tf.reshape(output2, [-1, 4 * 4 * 50])
    output3 = tf.matmul(output2, fc1_w) + fc1_b
    output3 = sess.run(output3)
    output3 = square(output3)
    output3 = output3
    output3 = tf.constant(output3, dtype=tf.float32)
    output4 = tf.matmul(output3, fc2_w) + fc2_b
    output4 = sess.run(output4)
    output4 = tf.nn.softmax(output4)
    pred = tf.argmax(output4, 1)
    print(sess.run(pred))
    correct_prediction = tf.equal(pred, tf.argmax(y_image, 1))
    accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))
    acc = sess.run(accuracy)
    print("the accuracy is: ", acc)
    # print(correct_prediction)


def run_raw_new_relu(x_image, conv1_w, conv1_b, conv2_w, conv2_b, fc1_w, fc1_b, fc2_w, fc2_b, y_image, sess):
    spaced_text = "test raw dataset relu"
    print(spaced_text.center(72, "#"))
    output1 = tf.nn.conv2d(x_image, conv1_w, strides=[1, 2, 2, 1], padding='VALID', use_cudnn_on_gpu=None) + conv1_b
    # output1 = sess.run(output1)
    output1 = tf.nn.avg_pool(tf.nn.relu(output1), ksize=[1, 3, 3, 1], strides=[1, 1, 1, 1], padding='SAME')

    output2 = tf.nn.conv2d(output1, conv2_w, strides=[1, 2, 2, 1], padding='VALID', use_cudnn_on_gpu=None) + conv2_b
    output2 = tf.nn.avg_pool(output2, ksize=[1, 3, 3, 1], strides=[1, 1, 1, 1], padding='SAME')

    output2 = tf.reshape(output2, [-1, 4 * 4 * 50])
    output3 = tf.nn.relu(tf.matmul(output2, fc1_w) + fc1_b)
    output4 = tf.nn.softmax(tf.matmul(output3, fc2_w) + fc2_b)
    pred = tf.argmax(output4, 1)
    print(sess.run(pred))
    correct_prediction = tf.equal(pred, tf.argmax(y_image, 1))
    accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))
    acc = sess.run(accuracy)
    print("the accuracy is: ", acc)


def square(x):
    return pow(x, 2)


def ff(x):
    y = -0.4089 * pow(x, 4) + 0 * pow(x, 3) + 0.8195 * pow(x, 2) + 0.5 * x + 0.05865
    return y


if __name__ == "__main__":
    # get_weight_bias()
    # spaced_text = "Encrypt all images"
    # print(spaced_text.center(72, "*"))
    mnist = input_data.read_data_sets("input_data", one_hot=True)
    now = time.time()
    # 加密图片，返回【1000，28，28，1】的形式的加密图片以及密钥。
    # encrypt_images, decryptor, encryptor, encoder, evaluator = encryt_mnist_test(mnist.test.images)
    # raw_figure = []
    # for each_image in encrypt_images:
    #     raw_figure.append(decrypt_and_decode_an_image(each_image, decryptor, encoder))
    # print(np.array(raw_figure).shape)
    print("Spend time: ", time.time() - now, "s\n")
    print("-------------------------Done----------------------------------\n")

    spaced_text = "test the model"
    print(spaced_text.center(72, "*"))
    get_weight_bias()
    print("-------------------------Done----------------------------------\n")

