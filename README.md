# Experiments

I have programmed experiments for my research projects since my undergraduate thesis. In the good old days, I used Visual Basic. Then, I used C++ and [libSDL](https://www.libsdl.org/) so that my experiments could be run on my PC and the Macs in the lab. Later I switched to C++ and [Direct2D](https://docs.microsoft.com/en-us/windows/win32/direct2d/direct2d-portal) for better graphic performance. During COVID-19, I moved my experiments online. At that time, JavaScript and the [Canvas API](https://developer.mozilla.org/en-US/docs/Web/API/Canvas_API) of HTML5 have already become mature enough for decent graphic performance for psychophysical experiments, even on an online platform.

I ran numerous experiments and cannot upload all of them here. However, I may showcase one of them at each stage. I am sorry that not much explanation may be given here about each experiment -- they usually involve looking for a predefined target among other objects, and the participants have to press a specific key (on the keyboard or click the mouse) when they find it.

## Dynamic visual search experiment

This is a class experiment I ran last year related to my [recently published paper](https://cognitiveresearchjournal.springeropen.com/articles/10.1186/s41235-022-00392-8). In this experiment, you look for a green circle among circles of other colours. What is unique about this experiment is that the stimuli are dynamic and move in and out from the view. This mimics real-life search scenarios better than traditional research of the same kind. This experiment is programmed in JavaScript and Canvas.

[[Code]](dynamic_search/) [[Online experiment]](https://louiskhchan.github.io/experiments/dynamic_search/index.htm?id=testuser)

## Target prevalence effect experiment

This is an experiment I ran in 2017. I presented the results at the 2017 Vision Sciences Society meeting [[abstract]](https://jov.arvojournals.org/article.aspx?articleid=2652004) [[pdf]](). This experiment studies how the frequency a target occurs would affect your decisions. Its result informs my later research on simulating human decisions using computer modelling techniques. This experiment is programmed in C++ with Direct2D.


[[Code]](target_prevalence/source/) [[Windows x86 build]](target_prevalence/build/)

## Attentional capture experiment

This is an experiment I ran in 2010-2014, and its results were published [in this paper](https://psycnet.apa.org/doi/10.1037/a0037897). This experiment studies how a salient object may distract you from a visual search task. It is unique because it measures accuracy at threshold exposure instead of reaction time. This experiment is programmed in C++ with libSDL.

[[Code]](attentional_capture/source/) [[Windows x86 build]](attentional_capture/build/)


