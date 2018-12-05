# -- coding: utf-8 --
import tensorflow as tf
import numpy as np
import tensorflow.examples.tutorials.mnist.input_data as input_data
from tensorflow.python.framework import ops
import time


def define_model(x):
    # define the structure of the neural network model
    x_image = tf.reshape(x, [-1, 28, 28, 1])
    # the first convolution layer
    with tf.variable_scope("conv1"):
        # weight
        W_conv1 = weight_variable([5, 5, 1, 5])
        # bias
        b_conv1 = bias_variable([5])
        h_conv1 = conv2d(x_image, W_conv1) + b_conv1
        # do convolution, and the activation function us ReLU. Input size is 28x28, output size 12x12x5
        h_conv1 = square_tf(h_conv1)
        # pooling, the output size is 12x12x32
        h_pool1 = avg_pool_3x3(h_conv1)

    # the second convolution layer
    with tf.variable_scope("conv2"):
        # weight
        W_conv2 = weight_variable([5, 5, 5, 50])
        # bias
        b_conv2 = bias_variable([50])
        # do convolution, output size 4x4x50
        h_conv2 = conv2d(h_pool1, W_conv2) + b_conv2
        # pooling, the output size is 4x4x50
        h_pool2 = avg_pool_3x3(h_conv2)

    # the first full connection layer
    with tf.variable_scope("fc1"):
        # weight
        W_fc1 = weight_variable([4 * 4 * 50, 100])
        # bias
        b_fc1 = bias_variable([100])
        # flat
        h_pool2_flat = tf.reshape(h_pool2, [-1, 4 * 4 * 50])
        # matrix multiplication, input size is 5*5*50, the output size is 100
        h_fc1 = square_tf(tf.matmul(h_pool2_flat, W_fc1) + b_fc1)
        h_fc1_drop = tf.nn.dropout(h_fc1, 0.5)

    # the second full connection layer
    with tf.variable_scope("fc2"):
        # weights
        W_fc2 = weight_variable([100, 10])
        # bias
        b_fc2 = bias_variable([10])
        # matrix multiplication and softmax, input size is 100, the output size is 10
        y_fc2 = tf.nn.softmax(tf.matmul(h_fc1_drop, W_fc2) + b_fc2)
    return y_fc2


def weight_variable(shape):
    # define the variable weight
    initial = tf.truncated_normal(shape=shape, stddev=0.1)
    return tf.Variable(initial, name="w")


def bias_variable(shape):
    # define the variable bias
    initial = tf.constant(0.1, shape=shape)
    return tf.Variable(initial, name="b")


def conv2d(x, W):
    # define convolution kernel
    # 卷积层，二维，步长[1，stride，stride，1]，same表示输出填充为原有大小
    return tf.nn.conv2d(x, W, strides=[1, 2, 2, 1], padding='VALID', use_cudnn_on_gpu=None)


def avg_pool_3x3(x):
    # define pooling size
    # 池化为3*3（shape是[batch, height, width, channels]）
    return tf.nn.avg_pool(x, ksize=[1, 3, 3, 1], strides=[1, 1, 1, 1], padding='SAME')


def square(x):
    return pow(x, 2)


def square_grad(x):
    return 2 * x


# making a common function into a numpy function
square_np = np.vectorize(square)
square_grad_np = np.vectorize(square_grad)

# numpy uses float64 but tensorflow uses float32
square_np_32 = lambda x: square_np(x).astype(np.float32)
square_grad_np_32 = lambda x: square_grad_np(x).astype(np.float32)


def square_grad_tf(x, name=None):
    with ops.name_scope(name, "square_grad_tf", [x]) as name:
        y = tf.py_func(square_grad_np_32, [x], [tf.float32], name=name, stateful=False)
        return y[0]


def my_py_func(func, inp, Tout, stateful=False, name=None, my_grad_func=None):
    # need to generate a unique name to avoid duplicates:
    random_name = "PyFuncGrad" + str(np.random.randint(0, 1E+8))
    tf.RegisterGradient(random_name)(my_grad_func)
    g = tf.get_default_graph()
    with g.gradient_override_map({"PyFunc": random_name, "PyFuncStateless": random_name}):
        return tf.py_func(func, inp, Tout, stateful=stateful, name=name)


def _square_grad(op, pred_grad):
    x = op.inputs[0]
    cur_grad = square_grad(x)
    next_grad = pred_grad * cur_grad
    return next_grad


def square_tf(x, name=None):
    with ops.name_scope(name, "square_tf", [x]) as name:
        y = my_py_func(square_np_32,
                       [x],
                       [tf.float32],
                       stateful=False,
                       name=name,
                       my_grad_func=_square_grad)
    return y[0]


def train_model():
    """
    train the NN
    :return:
    """
    # predefine
    x = tf.placeholder(tf.float32, shape=[None, 784], name='x')
    y_ = tf.placeholder(tf.float32, shape=[None, 10], name='y_')
    keep_prob = tf.placeholder(tf.float32, name="keep_prob")
    initial_learning_rate = 0.001
    y_fc2 = define_model(x)

    loss_temp = tf.losses.softmax_cross_entropy(onehot_labels=y_, logits=y_fc2)
    cross_entropy_loss = tf.reduce_mean(loss_temp)

    train_step = tf.train.AdamOptimizer(learning_rate=initial_learning_rate).minimize(cross_entropy_loss)

    correct_prediction = tf.equal(tf.argmax(y_fc2, 1), tf.argmax(y_, 1))

    accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32), name="acc")

    # save model
    saver = tf.train.Saver(max_to_keep=1)
    tf.add_to_collection("predict", y_fc2)

    with tf.Session() as sess:
        sess.run(tf.global_variables_initializer())
        print("--------------------train the model-------------------------")
        batch = mnist.train.next_batch(1024)
        for epoch in range(10000):
            start_time = time.time()
            train_step.run(feed_dict={x: batch[0], y_: batch[1], keep_prob: 0.5})
            batch = mnist.train.next_batch(1024)
            if (epoch + 1) % 10 == 0:
                train_accuracy = accuracy.eval(feed_dict={x: batch[0], y_: batch[1], keep_prob: 1.0})
                train_loss = cross_entropy_loss.eval(feed_dict={x: batch[0], y_: batch[1], keep_prob: 1.0})
                print("step %d, training accuracy %g" % (epoch + 1, train_accuracy))
                print("step %d, the loss is %g" % (epoch + 1, train_loss))
                saver.save(sess, 'model_5/mnist.ckpt', global_step=epoch + 1)
                print("spend_time " + str(time.time() - start_time) + "s")
                print("test accuracy %g" % accuracy.eval(feed_dict={x: mnist.test.images, y_: mnist.test.labels,
                                                                    keep_prob: 1.0}))
                # conv1_w = sess.run(sess.graph.get_tensor_by_name('conv1/w:0'))
                # print(conv1_w)


def load_model():
    # prepare the test data
    h = square_tf(1)
    print("load model".center(72, '*'))
    X_test = mnist.test.images
    Y_test = mnist.test.labels

    with tf.Session() as sess:
        # load the meta graph and weights
        saver = tf.train.import_meta_graph('model_5/mnist.ckpt-10000.meta')
        saver.restore(sess, tf.train.latest_checkpoint('model_5/'))
        y = tf.get_collection('predict')[0]
        graph = tf.get_default_graph()
        # print(sess.run(graph.get_tensor_by_name("conv1/w:0")))

        # 因为y中有placeholder，所以sess.run(y)的时候还需要用实际待预测的样本以及相应的参数来填充这些placeholder，而这些需要通过graph的get_operation_by_name方法来获取。
        input_x = graph.get_operation_by_name('x').outputs[0]
        input_y = graph.get_operation_by_name('y_').outputs[0]
        keep_prob = graph.get_operation_by_name('keep_prob').outputs[0]

        # 使用y进行预测
        pred = sess.run(y, feed_dict={input_x: X_test, keep_prob: 1.0})
        pred = tf.argmax(pred, 1)
        # print("the predict is ", pred)
        #
        correct_prediction = tf.equal(pred, tf.argmax(Y_test, 1))
        accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))
        acc = sess.run(accuracy)
        print("the accuracy is: ", acc)
        print("test accuracy %g" % accuracy.eval(feed_dict={
            input_x: mnist.test.images, input_y: mnist.test.labels, keep_prob: 1.0}))


if __name__ == "__main__":
    # 读取数据集
    mnist = input_data.read_data_sets("input_data", one_hot=True)
    now = time.time()
    train_model()
    # load_model()
    print(time.time() - now)
